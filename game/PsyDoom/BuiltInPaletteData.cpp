#include "BuiltInPaletteData.h"

#include "Doom/Renderer/r_data.h"

BEGIN_NAMESPACE(BuiltInPaletteData)

// Extra palettes for 'GEC Master Edition (Beta 3)'
const palette_t GEC_ME_BETA3_EXTRA_PALETTES[] = {
    // PAL 26: Credits screen - GEC logo
    { 
        0x0000, 0x8801, 0x8040, 0x8421, 0x8002, 0x8802, 0x8042, 0x8842, 0x8c21, 0x8c22, 0x9422, 0x8c42, 0x9442, 0x8060, 0x8861, 0x8481,
        0x8881, 0x8062, 0x8862, 0x8082, 0x8882, 0x8c61, 0x8c81, 0x8c62, 0x8c82, 0x8003, 0x8823, 0x8043, 0x8823, 0x8025, 0x8825, 0x8044,
        0x8844, 0x8c23, 0x9403, 0x8c43, 0x9443, 0x9005, 0x9425, 0x9045, 0x9045, 0x8064, 0x8863, 0x8483, 0x8883, 0x8064, 0x8865, 0x80a5,
        0x8884, 0x8c63, 0x9063, 0x8c83, 0x9084, 0x8c65, 0x9065, 0x8c84, 0x94a4, 0x9842, 0x9404, 0x9c23, 0x9843, 0x9c43, 0x9805, 0x9c04,
        0x9844, 0x9c44, 0x9c45, 0x9863, 0x9c64, 0x9864, 0x9c64, 0x9884, 0x9c85, 0x9c84, 0xa084, 0xa084, 0xa4a5, 0x88a1, 0x84e1, 0x88e1,
        0x84a2, 0x88c2, 0x88e2, 0x8cc2, 0x84e1, 0x8902, 0x8922, 0x8d02, 0x84c3, 0x88c3, 0x88e3, 0x80a5, 0x88c5, 0x8cc3, 0x90c3, 0x8ce3,
        0x94e3, 0x8cc4, 0x94c4, 0x8ce5, 0x94e4, 0x8d03, 0x9104, 0x8d23, 0x9123, 0x9104, 0x9104, 0x9524, 0x94e4, 0x94a5, 0x94e5, 0x9504,
        0x9d05, 0x9925, 0x8026, 0x8806, 0x8046, 0x8846, 0x8007, 0x8807, 0x8447, 0x8847, 0x8c06, 0x9006, 0x8c26, 0x9046, 0x8c27, 0x8c47,
        0x9427, 0x8065, 0x8866, 0x8086, 0x8886, 0x8067, 0x8867, 0x80a7, 0x88a6, 0x8c66, 0x9466, 0x8c86, 0x90a6, 0x8c67, 0x9067, 0x8c87,
        0x8028, 0x8448, 0x8848, 0x8029, 0x8449, 0x8849, 0x9048, 0x8488, 0x8868, 0x8488, 0x8888, 0x8869, 0x8889, 0x9088, 0x9805, 0xa026,
        0x9866, 0x9c86, 0x9885, 0x9c85, 0x9867, 0x9c87, 0x98a7, 0x9c87, 0xa066, 0xa466, 0xa0a5, 0xa486, 0xa067, 0xa467, 0xa087, 0xa4a7,
        0x9868, 0x9888, 0x9888, 0x9c88, 0x9489, 0xa088, 0xa468, 0xa487, 0x80c6, 0x88c6, 0x84e6, 0x88a7, 0x80e7, 0x88e6, 0x8cc6, 0x90c6,
        0x8ce6, 0x94e6, 0x8cc7, 0x90a7, 0x8ce7, 0x94e7, 0x9107, 0x88c8, 0x84e7, 0x90c8, 0x90c8, 0x90c8, 0x90c8, 0x90c9, 0x8ce9, 0x90e9,
        0x8908, 0x98c6, 0xa0c6, 0xa4a5, 0xa0c6, 0xa0c7, 0xa4c7, 0x9ce7, 0xa4e7, 0x9906, 0xa107, 0x98e8, 0xa0c8, 0x9909, 0xa107, 0xa508,
        0xa128, 0xa528, 0xa0ea, 0xa4e9, 0xa529, 0xa887, 0xa8c8, 0xa908, 0xa909, 0xa949, 0x8d43, 0x8d63, 0x9d47, 0x9d47, 0x9d47, 0xa949,
        0x884a, 0x8c8a, 0x906a, 0x8c8a, 0x94aa, 0x90aa, 0x98eb, 0x9d0a, 0x9cea, 0xa4ea, 0xa50b, 0xad6b, 0x8000, 0x8000, 0x8000, 0x8000
    },
    // PAL 27: Credits screen - Doomworld logo
    {
        0x0000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8420, 0x8420, 0x8420, 0x8420, 0x8420, 0x8420, 0x8420, 0x8420,
        0x8421, 0x8840, 0x8840, 0x8840, 0x8840, 0x8840, 0x8840, 0x8840, 0x8840, 0x8c60, 0x8c60, 0x8c60, 0x8c60, 0x8c60, 0x8c60, 0x8841,
        0x8841, 0x8841, 0x8841, 0x8841, 0x8c61, 0x8c61, 0x8c61, 0x8c61, 0x8c61, 0x8842, 0x8c62, 0x8c62, 0x8c62, 0x8c62, 0x8c62, 0x8c63,
        0x9080, 0x9080, 0x9080, 0x9080, 0x94a0, 0x94a0, 0x94a0, 0x94a0, 0x9081, 0x9081, 0x9081, 0x9081, 0x94a1, 0x94a1, 0x94a1, 0x98c0,
        0x98c0, 0x98c0, 0x98c0, 0x9ce0, 0x98c1, 0x98c1, 0x98c1, 0x9082, 0x9082, 0x9082, 0x9082, 0x9082, 0x94a2, 0x94a2, 0x94a2, 0x94a2,
        0x9083, 0x9083, 0x9083, 0x9083, 0x9083, 0x94a3, 0x94a3, 0x94a3, 0x94a3, 0x94a3, 0x98c2, 0x98c2, 0x98c2, 0x98c2, 0x9ce2, 0x9ce2,
        0x98c3, 0x98c3, 0x98c3, 0x98c3, 0x9ce3, 0x9ce3, 0x9084, 0x94a4, 0x94a4, 0x94a4, 0x94a4, 0x94a4, 0x94a4, 0x94a5, 0x98c4, 0x98c4,
        0x98c4, 0x98c4, 0x98c4, 0x9ce4, 0x9ce4, 0x9ce4, 0x9ce4, 0x98c5, 0x98c5, 0x98c5, 0x98c5, 0x98c5, 0x98c5, 0x9ce5, 0x9ce5, 0x9ce5,
        0x9ce5, 0x9ce5, 0x98c6, 0x9ce6, 0x9ce6, 0x9ce6, 0x9ce6, 0x9ce6, 0x9ce7, 0xa104, 0xa104, 0xa104, 0xa104, 0xa105, 0xa105, 0xa105,
        0xa105, 0xa525, 0xa106, 0xa106, 0xa106, 0xa106, 0xa106, 0xa106, 0xa526, 0xa526, 0xa526, 0xa526, 0xa107, 0xa107, 0xa107, 0xa107,
        0xa527, 0xa527, 0xa527, 0xa527, 0xa946, 0xa947, 0xa947, 0xad67, 0xa108, 0xa528, 0xa528, 0xa528, 0xa528, 0xa528, 0xa529, 0xa948,
        0xa948, 0xa948, 0xa948, 0xa948, 0xad68, 0xad68, 0xa949, 0xa949, 0xa949, 0xa949, 0xa949, 0xa949, 0xad69, 0xad69, 0xad69, 0xad69,
        0xad69, 0xa94a, 0xad6a, 0xad6a, 0xad6a, 0xad6a, 0xad6b, 0xb188, 0xb189, 0xb189, 0xb5a9, 0xb18a, 0xb18a, 0xb18a, 0xb18a, 0xb5aa,
        0xb5aa, 0xb5aa, 0xb18b, 0xb18b, 0xb18b, 0xb18b, 0xb18b, 0xb18b, 0xb5ab, 0xb5ab, 0xb5ab, 0xb9cb, 0xb18c, 0xb5ac, 0xb5ac, 0xb5ac,
        0xb5ac, 0xb5ac, 0xb5ac, 0xb5ad, 0xb9cc, 0xb9cc, 0xb9cc, 0xb9cc, 0xbdec, 0xb9cd, 0xb9cd, 0xb9cd, 0xb9cd, 0xb9cd, 0xbded, 0xbded,
        0xbded, 0xbded, 0xb9ce, 0xbdee, 0xbdee, 0xbdee, 0xbdee, 0xbdef, 0xc20e, 0xc20e, 0xc20f, 0xc20f, 0xc20f, 0xc20f, 0xc62f, 0xc62f 
    },
    // PAL 28: Credits screen - text for the GEC and Doomworld credits
    {
        0x0000, 0x8421, 0x800b, 0x842d, 0x8000, 0x8c71, 0x94b6, 0x8115, 0x8137, 0x98db, 0x8158, 0x859b, 0x89bb, 0x8ddb, 0xae7d, 0xcafe,
        0xdb5e, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000
    },
    // PAL 29: Main menu screen background
    {
        0x0000, 0x8421, 0x8800, 0x8c00, 0x8821, 0x8c21, 0x8841, 0x8c41, 0x8023, 0x8842, 0x8c42, 0x8c62, 0x8c63, 0x9021, 0x9421, 0x9421,
        0x9421, 0x9041, 0x9061, 0x9c41, 0x9822, 0x9442, 0x9062, 0x9443, 0x9063, 0x9842, 0x9842, 0x9c42, 0x9c63, 0x9481, 0x9ca3, 0x8405,
        0x8025, 0x8445, 0x8426, 0x8446, 0x9845, 0x8c84, 0x8ca5, 0x9084, 0x9084, 0x90a5, 0x9884, 0x90c6, 0x90c7, 0x90e7, 0x98c6, 0x98c6,
        0x9cc7, 0x9ce7, 0x9ce7, 0xa423, 0xa062, 0xa063, 0xa463, 0xa463, 0xa463, 0xac62, 0xa083, 0xb083, 0xa464, 0xb064, 0xa084, 0xa084,
        0xa4a5, 0xa884, 0xac84, 0xaca5, 0xa8c5, 0xaca6, 0xa8c6, 0xacc6, 0xace7, 0xb4a5, 0xb4c5, 0xb4e5, 0xb8c5, 0xb0c6, 0xb8e7, 0x8903,
        0x8903, 0x8d44, 0x8d44, 0x9185, 0x9185, 0x95a6, 0x95a6, 0x95e7, 0x95e7, 0xb566, 0x8428, 0x8848, 0x8869, 0x8c69, 0x886a, 0x8c6a,
        0x884b, 0x8ce8, 0x8c8b, 0x9488, 0x90e8, 0x988b, 0x98ca, 0x884c, 0x8c6d, 0x848c, 0x8c8c, 0x8c8d, 0x8ced, 0x908e, 0x90af, 0x90ce,
        0x90ef, 0x98cf, 0xbce8, 0x9109, 0x912a, 0x990a, 0x914b, 0x954b, 0x8d2e, 0x8d0f, 0x916c, 0x916d, 0x9d0f, 0x958d, 0x918e, 0x91ae,
        0x95ae, 0xa108, 0xa529, 0xa94a, 0xad6b, 0xb528, 0xb529, 0xb908, 0xbd29, 0xb549, 0xb52a, 0xbd4a, 0xbd8b, 0xb18c, 0xc485, 0xc0c7,
        0xc106, 0xc14a, 0xcd4b, 0xc58c, 0xc5ac, 0xcdac, 0xcdad, 0xc5cd, 0xd1ce, 0xd5ef, 0x9a28, 0x9a28, 0x9e69, 0x9e69, 0xa2aa, 0xa2aa,
        0xa6eb, 0xa6eb, 0xab2b, 0xab2b, 0xaf6c, 0xaf6c, 0x8000, 0x8c91, 0x8cf0, 0x8cd1, 0x94b0, 0x94b1, 0x94d1, 0x9cf1, 0x9093, 0x94d2,
        0x94d3, 0x94f3, 0x98d3, 0x94f4, 0x94d5, 0x98f4, 0x8d30, 0x8d12, 0x9511, 0x9531, 0x9d11, 0x9d31, 0x9551, 0x9533, 0x9d13, 0x9553,
        0x9573, 0x8db2, 0x9190, 0x91d0, 0x95d0, 0x91f1, 0x95f1, 0x95b3, 0x95d3, 0x99f3, 0x9514, 0x9535, 0x9554, 0x9574, 0x9555, 0x9d74,
        0x9d36, 0x95b4, 0x9595, 0x95d5, 0x95f5, 0x9596, 0x95d6, 0xa150, 0xa133, 0xa174, 0xa5d5, 0xa5b6, 0xa1d6, 0xb1d6, 0xfc1f, 0x9212,
        0x9612, 0x9613, 0x9233, 0x9633, 0x9634, 0x9235, 0x9635, 0x9254, 0x9655, 0x9275, 0x9675, 0x9656, 0x9276, 0x9676, 0x9676, 0x96b5,
        0x9297, 0x9697, 0x96b7, 0x9eb6, 0x9ed6, 0xa656, 0xaa56, 0xb637, 0xaa97, 0xc632, 0xda10, 0xde31, 0x8000, 0x8000, 0x8000, 0x8000
    },
    // PAL 30: 'DATA' sprite atlas lump in 'MEDOOM.WAD' (contains 'MASTER EDITION' text)
    {
        0x0000, 0x8108, 0x8cea, 0x8443, 0x8022, 0x992b, 0x910b, 0x88a6, 0x8421, 0x94ea, 0x952e, 0x9950, 0x8cc9, 0x910d, 0x9d92, 0xa9f9,
        0x8cc8, 0x88a7, 0x954f, 0xa1b5, 0xa5f8, 0x9971, 0x9db3, 0xa5d7, 0x8004, 0x800b, 0x9cfd, 0x8003, 0x8006, 0x8007, 0xa1b4, 0xae1a,
        0xb23b, 0xb65c, 0xb67d, 0x8c63, 0x98db, 0x94b9, 0x94b6, 0x9094, 0x8c71, 0x884f, 0x842d, 0x927e, 0xa6bd, 0x89d8, 0x852e, 0x850e,
        0x80ed, 0x80aa, 0x8089, 0x8067, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000
    },
};

// An extra palette for 'GEC Master Edition (Beta 4)' (and later).
// This is just here for the purposes of reserving the last palette slot.
// The actual palette data is uploaded dynamically later, during the map loading process.
const palette_t GEC_ME_DYNAMIC_PALETTE_PLACEHOLDERS[] = { palette_t{} };

// Sky palettes used by 'GEC Master Edition (Beta 4)' (and later).
// These are loaded into the dynamic sky palette slot during map loading.
const palette_t GEC_ME_MAP_SKY_PALETTES[] = {
    // SKY PALETTE 0
    {
        0x8000, 0x8443, 0x8022, 0xA529, 0xFFFF, 0x8C63, 0x8842, 0x8421, 0x8000, 0x8CC5, 0x84A4, 0x8062, 0x8041, 0x94E9, 0x90C8, 0x8CA7,
        0xDADF, 0xD29E, 0xCE7D, 0xCA5C, 0xC63C, 0xC21B, 0xB9DA, 0xB5B9, 0xB5B8, 0xB198, 0xAD77, 0xA956, 0xA535, 0xA115, 0x9CF4, 0x9CF3,
        0x98D2, 0x94B2, 0x94B1, 0x9090, 0x8C6F, 0x8C6F, 0x884E, 0x884D, 0x842D, 0x842C, 0x800B, 0x800B, 0x800A, 0x8009, 0x8008, 0x8007,
        0xEF9E, 0xE77E, 0xE35E, 0xDB3E, 0xD73E, 0xD31E, 0xCAFE, 0xC6DE, 0xBEBE, 0xBA9E, 0xB67D, 0xB65C, 0xB23B, 0xAE1A, 0xA9F9, 0xA5F8,
        0xA5D7, 0xA1B5, 0xA1B4, 0x9DB3, 0x9D92, 0x9971, 0x9950, 0x954F, 0x952E, 0x910D, 0x910B, 0x8CEA, 0x8CC9, 0x88A7, 0x88A6, 0x8485,
        0xF7BD, 0xF39C, 0xEF7B, 0xEB5A, 0xE739, 0xE318, 0xE318, 0xDEF7, 0xDAD6, 0xD6B5, 0xD294, 0xD294, 0xCE73, 0xCA52, 0xC631, 0xC210,
        0xBDEF, 0xBDEF, 0xB9CE, 0xB5AD, 0xB5AD, 0xB18C, 0xAD6B, 0xA94A, 0xA529, 0xA108, 0xA108, 0x9CE7, 0x98C6, 0x94A5, 0x9084, 0x9084,
        0xB76E, 0xB36D, 0xAF6C, 0xAB2B, 0xA6EB, 0xA2AA, 0x9E69, 0x9A28, 0x95E7, 0x95A6, 0x9185, 0x8D44, 0x8903, 0x84C2, 0x8482, 0x8041,
        0xC697, 0xC276, 0xBE55, 0xBA34, 0xB613, 0xB5F2, 0xB1D1, 0xADB0, 0xA9AF, 0xA58E, 0xA56E, 0xA14D, 0x9D4C, 0x992B, 0x990A, 0x94EA,
        0xB1F3, 0xA9D1, 0xA5AF, 0x9D6E, 0x994C, 0x950B, 0x90E9, 0x8CC8, 0xB1EE, 0xA9AD, 0xA5AC, 0xA18B, 0x9D4A, 0x9928, 0x9507, 0x90E6,
        0xC35A, 0xAF1A, 0xA2D9, 0x9657, 0x8DD5, 0x8972, 0x8110, 0x80AD, 0xFFFF, 0xEB5F, 0xDADF, 0xCA5F, 0xB9DF, 0xAD7F, 0x9CFF, 0x8C7F,
        0x843D, 0x843B, 0x843A, 0x8439, 0x8437, 0x8436, 0x8014, 0x8013, 0x8012, 0x8010, 0x800E, 0x800D, 0x800C, 0x800A, 0x8009, 0x8007,
        0xFF9C, 0xFF18, 0xFE94, 0xFE31, 0xFDAD, 0xFD4A, 0xFCC6, 0xFC63, 0xD000, 0xC800, 0xC000, 0xB800, 0xB000, 0xA800, 0xA000, 0x9800,
        0xFFFF, 0xEB9E, 0xDB5E, 0xCAFE, 0xBABE, 0xAE7D, 0x9E1D, 0x8DDB, 0x89BB, 0x85BA, 0x859B, 0x857A, 0x8158, 0x8137, 0x8116, 0x8115,
        0xFFFF, 0xEBBD, 0xDF9C, 0xCF7B, 0xBF5A, 0xAF39, 0x9AF7, 0x8252, 0x80F4, 0x80D3, 0x80B1, 0x8090, 0x90E9, 0x8CA8, 0x8886, 0x8465,
        0x9800, 0x9400, 0x9000, 0x8C00, 0x8800, 0x8400, 0x8000, 0x8000, 0xA25C, 0xAB19, 0xF9DE, 0xF81E, 0xE419, 0xC813, 0xB40D, 0x8000
    },
    // SKY PALETTE 1
    {
        0x8000, 0x8003, 0x8025, 0x8028, 0x804A, 0x806C, 0x806E, 0x8091, 0x80B3, 0x80F5, 0x8117, 0x8118, 0x813B, 0x815B, 0x815B, 0x817A,
        0x817A, 0x859A, 0x85B9, 0x85D9, 0x85F9, 0x8A19, 0x8A18, 0x8A38, 0x8E58, 0x8E77, 0x8E77, 0x9297, 0x9297, 0x96B7, 0x96B6, 0x96D6,
        0x9AD6, 0xB739, 0xCF7B, 0xE3BD, 0xFFFF, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0xFFFF
    },
    // SKY PALETTE 2
    {
        0x8000, 0xE318, 0xDAD6, 0xD6B5, 0xD294, 0xD294, 0xC276, 0xCE73, 0xAE1A, 0xBE55, 0xCA52, 0xA9F9, 0xBA34, 0xC631, 0xA5F8, 0xB613,
        0xC210, 0xA5D7, 0xB1F3, 0xB5F2, 0xBDEF, 0xB1D1, 0xBDEF, 0xA1B4, 0xADB0, 0xB9CE, 0x9DB3, 0xA9AF, 0x9D92, 0xB5AD, 0xB5AD, 0x9971,
        0xA58E, 0x8972, 0xA56E, 0xB18C, 0x8C7F, 0x9950, 0x954F, 0xAD6B, 0x885F, 0x885F, 0x843F, 0x952E, 0xA94A, 0x9D4C, 0x843F, 0x843F,
        0x843E, 0x843E, 0x843E, 0x843E, 0x843D, 0xA529, 0x843E, 0x910D, 0x843E, 0x843E, 0x843D, 0x843D, 0x843D, 0x843D, 0x843D, 0x843D,
        0x843C, 0x843C, 0x843C, 0x843C, 0x843C, 0x843C, 0x843C, 0x843C, 0x843B, 0x910B, 0x843B, 0x843B, 0x990A, 0x843B, 0xA108, 0x843B,
        0x843B, 0x843B, 0x843B, 0x843A, 0x843A, 0x843A, 0x843A, 0x843A, 0x94EA, 0xA108, 0x843A, 0x8439, 0x8439, 0x8439, 0x9507, 0x8CEA,
        0x8439, 0x8439, 0x8439, 0x8439, 0x8438, 0x8438, 0x94E9, 0x90E9, 0x8438, 0x90E9, 0x8438, 0x8437, 0x8437, 0x8437, 0x8437, 0x8436,
        0x9CE7, 0x8436, 0x8CC9, 0x8436, 0x8436, 0x8436, 0x8436, 0x8435, 0x8015, 0x90C8, 0x98C6, 0x8015, 0x8015, 0x8014, 0x8014, 0x8CC8,
        0x8014, 0x8014, 0x8014, 0x8014, 0x8013, 0x8013, 0x8013, 0x8013, 0x8013, 0x8013, 0x8013, 0x8012, 0x8012, 0x8012, 0x8012, 0x8012,
        0x8CA7, 0x8012, 0x94A5, 0x8012, 0x8012, 0x8011, 0x8011, 0x8011, 0x8011, 0x8011, 0x8011, 0x8011, 0x8011, 0x8011, 0x8010, 0x8010,
        0x8010, 0x88A6, 0x8010, 0x8010, 0x8010, 0x8010, 0x8010, 0x8010, 0x800F, 0x800F, 0x800F, 0x800F, 0x800F, 0x800F, 0x8886, 0x800F,
        0x9084, 0x800F, 0x800E, 0x800E, 0x800E, 0x800E, 0x800E, 0x800E, 0x800E, 0x800E, 0x800D, 0x800D, 0x800D, 0x8485, 0x800D, 0x9084,
        0x800D, 0x800D, 0x800D, 0x800D, 0x800D, 0x800D, 0x800C, 0x800C, 0x800C, 0x800C, 0x800C, 0x800C, 0x800C, 0x800C, 0x800B, 0x800B,
        0x800B, 0x800B, 0x800B, 0x800B, 0x800B, 0x800B, 0x800B, 0x800A, 0x800A, 0x800A, 0x800A, 0x800A, 0x800A, 0x8C63, 0x800A, 0x800A,
        0x800A, 0x8062, 0x800A, 0x8009, 0x8009, 0x8009, 0x8009, 0x8009, 0x8009, 0x8443, 0x8009, 0x8009, 0x8009, 0x8008, 0x8008, 0x8008,
        0x8008, 0x8008, 0x8008, 0x8008, 0x8008, 0x8008, 0x8007, 0x8007, 0x8842, 0x8041, 0x8022, 0x8421, 0x8000, 0x8400, 0x8000, 0x8000
    },
    // SKY PALETTE 3
    {
        0x8000, 0x8443, 0x8022, 0xA529, 0xFFFF, 0x8C63, 0x8842, 0x8421, 0x8000, 0x8CC5, 0x84A4, 0x8062, 0x8041, 0x94E9, 0x90C8, 0x8CA7,
        0xDADF, 0xD29E, 0xCE7D, 0xCA5C, 0xC63C, 0xC21B, 0xB9DA, 0xB5B9, 0xB5B8, 0xB198, 0xAD77, 0xA956, 0xA535, 0xA115, 0x9CF4, 0x9CF3,
        0x98D2, 0x94B2, 0x94B1, 0x9090, 0x8C6F, 0x8C6F, 0x884E, 0x884D, 0x842D, 0x842C, 0x800B, 0x800B, 0x800A, 0x8009, 0x8008, 0x8007,
        0xEF9E, 0xE77E, 0xE35E, 0xDB3E, 0xD73E, 0xD31E, 0xCAFE, 0xC6DE, 0xBEBE, 0xBA9E, 0xB67D, 0xB65C, 0xB23B, 0xAE1A, 0xA9F9, 0xA5F8,
        0xA5D7, 0xA1B5, 0xA1B4, 0x9DB3, 0x9D92, 0x9971, 0x9950, 0x954F, 0x952E, 0x910D, 0x910B, 0x8CEA, 0x8CC9, 0x88A7, 0x88A6, 0x8485,
        0xF7BD, 0xF39C, 0xEF7B, 0xEB5A, 0xE739, 0xE318, 0xE318, 0xDEF7, 0xDAD6, 0xD6B5, 0xD294, 0xD294, 0xCE73, 0xCA52, 0xC631, 0xC210,
        0xBDEF, 0xBDEF, 0xB9CE, 0xB5AD, 0xB5AD, 0xB18C, 0xAD6B, 0xA94A, 0xA529, 0xA108, 0xA108, 0x9CE7, 0x98C6, 0x94A5, 0x9084, 0x9084,
        0xB76E, 0xB36D, 0xAF6C, 0xAB2B, 0xA6EB, 0xA2AA, 0x9E69, 0x9A28, 0x95E7, 0x95A6, 0x9185, 0x8D44, 0x8903, 0x84C2, 0x8482, 0x8041,
        0xC697, 0xC276, 0xBE55, 0xBA34, 0xB613, 0xB5F2, 0xB1D1, 0xADB0, 0xA9AF, 0xA58E, 0xA56E, 0xA14D, 0x9D4C, 0x992B, 0x990A, 0x94EA,
        0xB1F3, 0xA9D1, 0xA5AF, 0x9D6E, 0x994C, 0x950B, 0x90E9, 0x8CC8, 0xB1EE, 0xA9AD, 0xA5AC, 0xA18B, 0x9D4A, 0x9928, 0x9507, 0x90E6,
        0xC35A, 0xAF1A, 0xA2D9, 0x9657, 0x8DD5, 0x8972, 0x8110, 0x80AD, 0xFFFF, 0xEB5F, 0xDADF, 0xCA5F, 0xB9DF, 0xAD7F, 0x9CFF, 0x8C7F,
        0x843D, 0x843B, 0x843A, 0x8439, 0x8437, 0x8436, 0x8014, 0x8013, 0x8012, 0x8010, 0x800E, 0x800D, 0x800C, 0x800A, 0x8009, 0x8007,
        0xFF9C, 0xFF18, 0xFE94, 0xFE31, 0xFDAD, 0xFD4A, 0xFCC6, 0xFC63, 0xD000, 0xC800, 0xC000, 0xB800, 0xB000, 0xA800, 0xA000, 0x9800,
        0xFFFF, 0xEB9E, 0xDB5E, 0xCAFE, 0xBABE, 0xAE7D, 0x9E1D, 0x8DDB, 0x89BB, 0x85BA, 0x859B, 0x857A, 0x8158, 0x8137, 0x8116, 0x8115,
        0xFFFF, 0xEBBD, 0xDF9C, 0xCF7B, 0xBF5A, 0xAF39, 0x9AF7, 0x8252, 0x80F4, 0x80D3, 0x80B1, 0x8090, 0x90E9, 0x8CA8, 0x8886, 0x8465,
        0x9800, 0x9400, 0x9000, 0x8C00, 0x8800, 0x8400, 0x8000, 0x8000, 0xA25C, 0xAB19, 0xF9DE, 0xF81E, 0xE419, 0xC813, 0xB40D, 0x8000
    },
    // SKY PALETTE 4
    {
        0x8000, 0x9992, 0x9991, 0xA5AE, 0xA590, 0xA58E, 0xA18E, 0x9551, 0x9951, 0xA18D, 0x9551, 0xA14E, 0x9550, 0xA14D, 0xA14D, 0x994D,
        0x9530, 0x9130, 0xA14D, 0x992E, 0x952E, 0xA12D, 0x912E, 0x992D, 0x952D, 0xA12D, 0x912D, 0x992D, 0x952D, 0xA12C, 0x910E, 0x992C,
        0x950D, 0x952C, 0x910D, 0x990D, 0x992A, 0x950D, 0x910D, 0x990C, 0x910D, 0x950C, 0x910C, 0x910C, 0x990A, 0x950A, 0x910D, 0x910A,
        0x910D, 0x950C, 0x910C, 0x910C, 0x990A, 0x950A, 0x910A, 0x910A, 0x9509, 0x890A, 0x9109, 0x9109, 0x8909, 0x90CA, 0x94C9, 0x88CA,
        0x90C9, 0x90C9, 0x88C9, 0x90C8, 0x90C8, 0x88C8, 0x90A9, 0x88A9, 0x90A8, 0x90A8, 0x88A8, 0x90A8, 0x90A8, 0x88A8, 0x84A8, 0x88A6,
        0x84A6, 0x9088, 0x8888, 0x8488, 0x9086, 0x8886, 0x8486, 0x8885, 0x8485, 0x8886, 0x8486, 0x8484, 0x8885, 0x8485, 0x8085, 0x8884,
        0x8484, 0x8084, 0x8884, 0x8484, 0x8084, 0x8045, 0x8844, 0x8444, 0x8044, 0x8844, 0x8444, 0x8044, 0x8842, 0x8442, 0x8042, 0x8006,
        0x8424, 0x8441, 0x8024, 0x8007, 0x8422, 0x8022, 0x8821, 0x8022, 0x8421, 0x8021, 0x8420, 0x8002, 0x8020, 0x8401, 0x8001, 0x8400,
        0x8000, 0x8000, 0xA5B0, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000
    },
    // SKY PALETTE 5
    {
        0x8000, 0xFBDE, 0xFBDE, 0xFBDE, 0xEBBD, 0xFFDF, 0xFBDF, 0xF7DE, 0xFFDE, 0xF3BE, 0xFBDE, 0xF7DD, 0xF7BF, 0xF3BF, 0xDF9D, 0xEB9E,
        0xFBBD, 0xF7BE, 0xEB9E, 0xF3BE, 0xBF5A, 0xF7BD, 0xF3BD, 0xEB7E, 0xCB7C, 0xE77E, 0xEB7F, 0xAB18, 0xEF7C, 0xE75F, 0xE35E, 0xE35F,
        0xDB3E, 0xE75D, 0xDB3E, 0xDB5D, 0xDB3F, 0xDB3E, 0xD31E, 0xE75A, 0xDB3C, 0xAF19, 0xCAFE, 0xDAFE, 0xD2FE, 0xCB1D, 0xCADE, 0xD71B,
        0xC6DE, 0xE318, 0xCADD, 0xD6DE, 0xD6DB, 0xD2DB, 0xCABD, 0xD29E, 0xD6D8, 0xCABA, 0xBE9D, 0xA2D9, 0xC65C, 0xC6B8, 0xCE97, 0xC698,
        0xC697, 0xD295, 0xC65A, 0xC696, 0xC296, 0xC296, 0xB23A, 0xC257, 0xC675, 0xC256, 0xCE73, 0xBE1A, 0xBA56, 0xBA55, 0xC236, 0x9258,
        0xC254, 0xBA35, 0xB636, 0xBA34, 0xB634, 0xAE17, 0xBA34, 0xC631, 0xB634, 0xB632, 0xB614, 0xB214, 0xBA12, 0xC210, 0xB612, 0xB1D5,
        0xB212, 0xBA11, 0xA1D6, 0xB5D2, 0xB5D1, 0xADD2, 0xB1D1, 0x89D5, 0xA9D1, 0xA9D1, 0xA1B5, 0xB1D1, 0xB1B1, 0xA1B3, 0xA9B1, 0xB1CF,
        0x99B4, 0xA5B1, 0xA9B1, 0xB5AD, 0xB5AD, 0xA9AF, 0x9992, 0xA5AF, 0xA1B0, 0xB5AD, 0xADAD, 0x9D71, 0xB5AD, 0xA190, 0xA58F, 0xA58D,
        0xA18E, 0x9D8F, 0x8552, 0xA18D, 0xA1AC, 0x9551, 0xB18C, 0x9D4F, 0xA96C, 0xA14D, 0xA16D, 0x952F, 0xA16B, 0xA54B, 0xA16A, 0x994D,
        0xA94A, 0x912E, 0xA12D, 0xA12C, 0xA12B, 0xA52B, 0x992C, 0x952C, 0x910D, 0xA12A, 0xA529, 0xA129, 0x994A, 0x990C, 0x8110, 0x9D0A,
        0xA109, 0x9D09, 0x910C, 0x950A, 0xA108, 0xA108, 0xA108, 0x9928, 0xA108, 0x910A, 0x9D09, 0x910A, 0x9509, 0xA108, 0xA108, 0x9908,
        0x9508, 0xA108, 0x9908, 0xA106, 0x9507, 0x90CA, 0x9906, 0x98C8, 0x94C8, 0x88C9, 0xA0C8, 0x9106, 0x90E8, 0x98C8, 0x90C8, 0x94C8,
        0xA0C6, 0x98C6, 0x94C6, 0x90A8, 0x94A8, 0x98C5, 0x88A8, 0x90A8, 0x90C6, 0x90A8, 0x98A6, 0x94A6, 0x88A8, 0x90A6, 0x98A5, 0x94A5,
        0x8CA6, 0x90A5, 0x94A4, 0x90A4, 0x9086, 0x8CA4, 0x8886, 0x9485, 0x9085, 0x804C, 0x8486, 0x9085, 0x9084, 0x8868, 0x8485, 0x8885,
        0x9084, 0x8886, 0x8884, 0x9084, 0x9084, 0x8485, 0x9084, 0x9084, 0x8884, 0x9084, 0x8464, 0x8084, 0x8883, 0x9082, 0x8464, 0x8844,
        0x8063, 0x8842, 0x8442, 0x8024, 0x8822, 0x8422, 0x8022, 0x8421, 0x8021, 0x8001, 0x8001, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000
    },
    // SKY PALETTE 6
    {
        0x8000, 0x8421, 0x8041, 0x8421, 0x8421, 0x8421, 0x8421, 0x8421, 0x8842, 0x8842, 0x8042, 0x8842, 0x8842, 0x8842, 0x8842, 0x8042,
        0x8842, 0x8842, 0x8C63, 0x8043, 0x8C63, 0x8C63, 0x8C63, 0x8043, 0x8C63, 0x8463, 0x8C63, 0x8483, 0x8463, 0x8C63, 0x88A3, 0x9084,
        0x8064, 0x9084, 0x8484, 0x8464, 0x9084, 0x9084, 0x8004, 0x8004, 0x8004, 0x8004, 0x8004, 0x8064, 0x8464, 0x8464, 0x9084, 0x9084,
        0x8484, 0x94A5, 0x8005, 0x8025, 0x8485, 0x8485, 0x8485, 0x8885, 0x94A5, 0x8CC5, 0x8485, 0x8006, 0x8006, 0x8886, 0x84A6, 0x8006,
        0x8006, 0x8486, 0x88A6, 0x8CA6, 0x8026, 0x8026, 0x98C6, 0x8026, 0x8886, 0x84A6, 0x88A6, 0x8006, 0x8006, 0x8006, 0x8026, 0x84C6,
        0x98C6, 0x8027, 0x8027, 0x8027, 0x8027, 0x8027, 0x8CA7, 0x8027, 0x88C7, 0x8CC7, 0x9507, 0x8007, 0x8007, 0x9CE7, 0x8027, 0x90E7,
        0x8027, 0x8027, 0x8CC7, 0x8CC7, 0x8027, 0x8027, 0x8027, 0x84C7, 0x90C7, 0x8048, 0x8CC8, 0x88C8, 0x8CE8, 0xA108, 0x8008, 0x8008,
        0x8028, 0x8028, 0x8028, 0x8028, 0x8028, 0x8CE8, 0x8028, 0x8028, 0x8048, 0x8048, 0x84C8, 0x90E8, 0x8009, 0x8009, 0x8CE9, 0xA529,
        0x8009, 0x8029, 0x8029, 0x90E9, 0x8029, 0x8029, 0x8D09, 0x8049, 0x8049, 0x8049, 0x8049, 0x8049, 0x8049, 0x9509, 0x8049, 0x88EA,
        0x950A, 0x800A, 0x910A, 0x802A, 0x992A, 0x800A, 0x802A, 0x802A, 0x950A, 0x802A, 0x802A, 0x804A, 0x804A, 0x804A, 0x804A, 0x804A,
        0x804A, 0x804A, 0x804A, 0xA54A, 0xA94A, 0x800B, 0x804B, 0x802B, 0x802B, 0x804B, 0x802B, 0x804B, 0x806B, 0x992B, 0x804B, 0x804B,
        0x804B, 0x804B, 0x910B, 0x804B, 0x804B, 0x804B, 0x800B, 0x802B, 0x806B, 0x912B, 0xAD6B, 0x802C, 0x806C, 0x992C, 0x804C, 0x804C,
        0x804C, 0x804C, 0x804C, 0x804C, 0x804C, 0x804C, 0x806C, 0x806C, 0x842C, 0x802C, 0x802C, 0x802C, 0x804C, 0x9D4C, 0x804D, 0x804D,
        0x806D, 0xB5AD, 0x804D, 0x804D, 0x806D, 0x804D, 0x842D, 0x804D, 0x806D, 0x806D, 0x806D, 0x806D, 0x802D, 0x804D, 0x806D, 0x806D,
        0x808D, 0x804D, 0x808D, 0x9D6D, 0x806E, 0x806E, 0x842E, 0x806E, 0x806E, 0x806E, 0x808E, 0x842E, 0x804E, 0xA58E, 0x806F, 0xB5CF,
        0x806F, 0x808F, 0x808F, 0xBDEF, 0x808F, 0x808F, 0xADB0, 0x8430, 0x8090, 0x8450, 0xC631, 0x8072, 0x8000, 0x8000, 0x8000, 0x8000
    },
};

// The size of each palette list
const uint8_t NUM_GEC_ME_BETA3_EXTRA_PALETTES = C_ARRAY_SIZE(GEC_ME_BETA3_EXTRA_PALETTES);
const uint8_t NUM_GEC_ME_DYNAMIC_PALETTE_PLACEHOLDERS = C_ARRAY_SIZE(GEC_ME_DYNAMIC_PALETTE_PLACEHOLDERS);
const uint8_t NUM_GEC_ME_MAP_SKY_PALETTES = C_ARRAY_SIZE(GEC_ME_MAP_SKY_PALETTES);

END_NAMESPACE(BuiltInPaletteData)