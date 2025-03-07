#include "WadFile.h"

#include "Doom/Base/i_main.h"
#include "Doom/Base/z_zone.h"
#include "Doom/d_main.h"
#include "FileUtils.h"
#include "WadUtils.h"

#include <cctype>

//------------------------------------------------------------------------------------------------------------------------------------------
// Header for a WAD file: constains high level information about the contents of the WAD
//------------------------------------------------------------------------------------------------------------------------------------------
struct WadHdr {
    char        fileid[4];          // Should always be "IWAD" for PSX DOOM
    int32_t     numLumps;           // The number of lumps in the WAD
    int32_t     lumpHdrsOffset;     // Offset in the WAD of the array of lump headers
};

static_assert(sizeof(WadHdr) == 12);

//------------------------------------------------------------------------------------------------------------------------------------------
// Header for a lump in a WAD file
//------------------------------------------------------------------------------------------------------------------------------------------
struct WadLumpHdr {
    uint32_t        wadFileOffset;
    uint32_t        uncompressedSize;
    WadLumpName     name;
};

static_assert(sizeof(WadLumpHdr) == 16);

//------------------------------------------------------------------------------------------------------------------------------------------
// Creates a WAD file that has not yet been opened
//------------------------------------------------------------------------------------------------------------------------------------------
WadFile::WadFile() noexcept
    : mNumLumps(0)
    , mSizeInBytes(0)
    , mLumpNames{}
    , mLumps{}
    , mFileReader()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Move a WAD file from one object to another
//------------------------------------------------------------------------------------------------------------------------------------------
WadFile::WadFile(WadFile&& other) noexcept 
    : mNumLumps(other.mNumLumps)
    , mSizeInBytes(other.mSizeInBytes)
    , mLumpNames(std::move(other.mLumpNames))
    , mLumps(std::move(other.mLumps))
    , mFileReader(std::move(other.mFileReader))
{
    other.mNumLumps = 0;
    other.mSizeInBytes = 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Frees up the WAD file and it's resources
//------------------------------------------------------------------------------------------------------------------------------------------
WadFile::~WadFile() noexcept {
    close();
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Closes up the WAD file and releases all resources used
//------------------------------------------------------------------------------------------------------------------------------------------
void WadFile::close() noexcept {
    purgeAllLumps();

    mFileReader.close();
    mLumps.reset();
    mLumpNames.reset();
    mSizeInBytes = 0;
    mNumLumps = 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Opens the specified WAD file on disk.
// Note: if a WAD file is currently opened then it is closed first.
//------------------------------------------------------------------------------------------------------------------------------------------
void WadFile::open(const char* const filePath, const RemapWadLumpNameFn lumpNameRemapFn) noexcept {
    close();

    // Sanity check the size of the WAD file
    const int64_t fileSize = FileUtils::getFileSize(filePath);

    if (fileSize <= 0)
        FatalErrors::raiseF("WadFile::open: error opening file '%s'!", filePath);

    if (fileSize > INT32_MAX)
        FatalErrors::raiseF("WadFile::open: file '%s' is too big!", filePath);

    // Open the WAD file and perform all other initialization
    mSizeInBytes = (int32_t) fileSize;
    mFileReader.open(filePath);
    initAfterOpen(lumpNameRemapFn);
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Opens the specified WAD file from the game's CD.
// Note: if a WAD file is currently opened then it is closed first.
//------------------------------------------------------------------------------------------------------------------------------------------
void WadFile::open(const CdFileId fileId, const RemapWadLumpNameFn lumpNameRemapFn) noexcept {
    close();
    
    // Open the WAD file and sanity check its size
    const PsxCd_File& file = mFileReader.open(fileId);
    
    if (file.size <= 0)
        FatalErrors::raiseF("WadFile::open: bad file size (<= 0) for wad file '%s'!", fileId.c_str().data());
    
    mSizeInBytes = file.size;

    // Perform all other initialization
    initAfterOpen(lumpNameRemapFn);
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Returns the index of the lump with the specified name, starting the search from the specified lump index.
// If not found then '-1' will be returned.
// Note: when searching the 'compressed' flag bit in the 1st byte of candidate lump names is ignored.
//------------------------------------------------------------------------------------------------------------------------------------------
int32_t WadFile::findLumpIdx(const WadLumpName lumpName, const int32_t searchStartIdx) const noexcept {
    const int32_t numLumps = mNumLumps;
    const WadLumpName* const pLumpNames = mLumpNames.get();

    for (int32_t i = searchStartIdx; i < numLumps; ++i) {
        if ((pLumpNames[i].word() & WAD_LUMPNAME_MASK) == lumpName.word())
            return i;
    }

    return -1;
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Evict a single lump from the cache (if cached) and free the memory used to hold it's data
//------------------------------------------------------------------------------------------------------------------------------------------
void WadFile::purgeCachedLump(const int32_t lumpIdx) noexcept {
    ASSERT(isValidLumpIdx(lumpIdx));

    WadLump& lump = mLumps[lumpIdx];
    void* const pCachedData = lump.pCachedData;

    if (pCachedData) {
        Z_Free2(*gpMainMemZone, pCachedData);
        ASSERT_LOG(!lump.pCachedData, "Z_Free2 should clear the pointer field pointing to the freed memory block!");
        lump.bIsUncompressed = false;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Evicts all lumps from the cache and frees the memory used to hold their data
//------------------------------------------------------------------------------------------------------------------------------------------
void WadFile::purgeAllLumps() noexcept {
    WadLump* const pLumps = mLumps.get();
    const int32_t numLumps = mNumLumps;
    memzone_t& mainMemZone = *gpMainMemZone;

    for (int32_t i = 0; i < numLumps; ++i) {
        void* const pCachedData = pLumps[i].pCachedData;

        if (pCachedData) {
            Z_Free2(mainMemZone, pCachedData);
            ASSERT_LOG(!pLumps[i].pCachedData, "Z_Free2 should clear the pointer field pointing to the freed memory block!");
            pLumps[i].bIsUncompressed = false;
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Returns the size in bytes that the lump occupies in the WAD file.
// If the lump is uncompressed then this will be the actual lump size.
// If the lump is compressed then this will be the compressed size.
//------------------------------------------------------------------------------------------------------------------------------------------
int32_t WadFile::getRawSize(const int32_t lumpIdx) noexcept {
    ASSERT(isValidLumpIdx(lumpIdx));

    // In most cases use the distance between the specified lump and the next one to tell the raw size.
    // If there is no next lump however then use the distance to the end of the WAD file instead.
    const int32_t thisLumpOffset = mLumps[lumpIdx].wadFileOffset;
    const int32_t nextLumpOffset = (lumpIdx + 1 < mNumLumps) ? mLumps[lumpIdx + 1].wadFileOffset : mSizeInBytes;

    return (nextLumpOffset > thisLumpOffset) ? (int32_t)(nextLumpOffset - thisLumpOffset) : 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Cache/load the specified lump index and return all of the lump information, including the cached data for the lump.
// Optionally, the lump can also be decompressed.
// 
// Notes:
//  (1) If the lump is already cached then this call is a no-op, unless if the cached lump is compressed and decompression is required.
//  (2) If decompression is not required then an uncompressed lump data may still be returned. This can happen if the data in the WAD file
//      is already decompressed or if the lump was cached in a decompressed state previously.
//------------------------------------------------------------------------------------------------------------------------------------------
const WadLump& WadFile::cacheLump(const int32_t lumpIdx, const int16_t allocTag, const bool bDecompress) noexcept {
    ASSERT(isValidLumpIdx(lumpIdx));

    // See if the lump is already loaded.
    // If that is the case then this call might just be a no-op.
    WadLump& lump = mLumps[lumpIdx];

    if (lump.pCachedData) {
        // Handle the situation where the caller wanted the data uncompressed when it's already loaded and compressed.
        // In this scenario decompress the currently loaded lump and have it replace the previously compressed version of the lump.
        // The original game did not trigger this case but now with PsyDoom it's possible, so we must fix.
        if (bDecompress && (!lump.bIsUncompressed)) {
            void* const pCompressedLump = lump.pCachedData;
            
            Z_SetUser(pCompressedLump, nullptr); // N.B: Doing this to avoid wiping the cache entry on 'Z_Free'
            Z_Malloc(*gpMainMemZone, lump.uncompressedSize, allocTag, &lump.pCachedData);
            WadUtils::decompressLump(pCompressedLump, lump.pCachedData);
            Z_Free2(*gpMainMemZone, pCompressedLump);

            lump.bIsUncompressed = true;
        }

        return lump;
    }

    // If we get to here then the lump is not cached and will have to be loaded.
    // Note that originally the PSX engine disallowed loading lumps during gameplay due to slow CD-ROM I/O, but PsyDoom waives this restriction.
    // This change means that levels no longer need to ship with 'MAPSPR--.IMG' and 'MAPTEX--.IMG' files and can load resources on the fly.
    int32_t sizeToRead;

    if (bDecompress) {
        sizeToRead = lump.uncompressedSize;
    } else {
        sizeToRead = getRawSize(lumpIdx);
    }

    // Alloc RAM for the lump and read it
    Z_Malloc(*gpMainMemZone, sizeToRead, allocTag, &lump.pCachedData);
    readLump(lumpIdx, lump.pCachedData, bDecompress);

    // Save whether the lump is compressed or not.
    // If the lump is compressed then the highest bit of the first character in the name will be set:
    if ((uint8_t) mLumpNames[lumpIdx].chars[0] & 0x80) {
        lump.bIsUncompressed = bDecompress;
    } else {
        lump.bIsUncompressed = true;
    }

    return lump;
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Reads the requested lump index into the given buffer.
// The buffer must be big enough to accomodate the data and (optionally) decompression can be disabled.
//------------------------------------------------------------------------------------------------------------------------------------------
void WadFile::readLump(const int32_t lumpIdx, void* const pDest, const bool bDecompress) noexcept {
    ASSERT(isValidLumpIdx(lumpIdx));

    // Get info for the lump requested and the one following it
    const WadLump& lump = mLumps[lumpIdx];
    const WadLumpName& lumpName = mLumpNames[lumpIdx];

    // Do we need to decompress?
    // Decompress if specified and if the lumpname has the special bit set in the first character, indicating that the lump is compressed.
    const uint32_t sizeToRead = getRawSize(lumpIdx);
    const bool bIsLumpCompressed = ((uint8_t) lumpName.chars[0] & 0x80u);

    if (bDecompress && bIsLumpCompressed) {
        // Decompression needed, must alloc a temp buffer for the compressed data before reading and decompressing!
        void* const pTmpBuffer = Z_EndMalloc(*gpMainMemZone, sizeToRead, PU_STATIC, nullptr);

        mFileReader.seekAbsolute(lump.wadFileOffset);
        mFileReader.read(pTmpBuffer, sizeToRead);
        ASSERT(WadUtils::getDecompressedLumpSize(pTmpBuffer) == lump.uncompressedSize);   // Sanity check the WAD data in debug mode
        WadUtils::decompressLump(pTmpBuffer, pDest);

        Z_Free2(*gpMainMemZone, pTmpBuffer);
    } else {
        // No decompression needed, can just read straight into the output buffer
        mFileReader.seekAbsolute(lump.wadFileOffset);
        mFileReader.read(pDest, sizeToRead);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Performs WAD initialization after the file has been opened
//------------------------------------------------------------------------------------------------------------------------------------------
void WadFile::initAfterOpen(const RemapWadLumpNameFn lumpNameRemapFn) noexcept {
    readLumpInfo(lumpNameRemapFn);
}

//------------------------------------------------------------------------------------------------------------------------------------------
// WAD file initialization step: reads the header for the WAD, then reads information for all lumps and sets up lump entries
//------------------------------------------------------------------------------------------------------------------------------------------
void WadFile::readLumpInfo(const RemapWadLumpNameFn lumpNameRemapFn) noexcept {
    // Read and ensure the WAD file header is valid.
    // Note: allow 'IWAD' or 'PWAD' as the file id, originally just 'IWAD' was allowed.
    WadHdr wadHdr = {};
    mFileReader.read(wadHdr);

    const bool bIsIWAD = (D_strncasecmp(wadHdr.fileid, "IWAD", sizeof(wadHdr.fileid)) == 0);
    const bool bIsPWAD = (D_strncasecmp(wadHdr.fileid, "PWAD", sizeof(wadHdr.fileid)) == 0);

    if ((!bIsIWAD) && (!bIsPWAD)) {
        I_Error("WadFile::initAfterOpen: invalid file id for WAD! Must be 'IWAD' or 'PWAD'!");
    }

    // Read the array of lump headers
    std::unique_ptr<WadLumpHdr[]> lumpHdrs(new WadLumpHdr[wadHdr.numLumps]);
    mFileReader.seekAbsolute(wadHdr.lumpHdrsOffset);
    mFileReader.read(lumpHdrs.get(), wadHdr.numLumps * sizeof(WadLumpHdr));

    // Setup the list of lump names and lumps using the lump headers
    mNumLumps = wadHdr.numLumps;
    mLumpNames.reset(new WadLumpName[wadHdr.numLumps]);
    mLumps.reset(new WadLump[wadHdr.numLumps]);
    
    std::memset(mLumps.get(), 0, sizeof(WadLump) * wadHdr.numLumps);

    for (int32_t i = 0; i < wadHdr.numLumps; ++i) {
        const WadLumpHdr& lumpHdr = lumpHdrs[i];
        WadLump& lump = mLumps[i];

        // Remap the lump name if a name remapper is supplied!
        // When remapping however don't supply the 'compressed' flag encoded in the lump name.
        // Also re-add it after remapping, don't allow the remapping code to have any say over whether something is compressed:
        if (lumpNameRemapFn) {
            // Games like 'GEC Master Edition (Beta 3)' need some lump remapping to work!
            const uint64_t compressedLumpBit = lumpHdr.name.word() & (~WAD_LUMPNAME_MASK);
            mLumpNames[i] = lumpNameRemapFn(lumpHdr.name.word() ^ compressedLumpBit).word() ^ compressedLumpBit;
        } else {
            // This is the normal case for the 'Doom' and 'Final Doom' games - no lump remapper used:
            mLumpNames[i] = lumpHdr.name;
        }

        lump.wadFileOffset = lumpHdr.wadFileOffset;
        lump.uncompressedSize = lumpHdr.uncompressedSize;
    }
}
