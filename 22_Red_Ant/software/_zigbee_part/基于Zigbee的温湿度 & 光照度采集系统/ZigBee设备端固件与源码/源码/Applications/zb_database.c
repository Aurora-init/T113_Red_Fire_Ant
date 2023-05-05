#include "zb_database.h"

#include "hal_m25pexx.h"

#define ZB_DATABASE_ADDR    0x1000

static void zb_database_init(void);
static bool zb_database_write(uint8_t *buf, uint8_t len);
static bool zb_database_read(uint8_t *buf, uint8_t len);

const zb_database_t *zbdb_ = &(const zb_database_t){
    .init = zb_database_init,
    .write = zb_database_write,
    .read = zb_database_read,
};

void zb_database_init()
{
    halM25PExxInit();
}

bool zb_database_write(uint8_t *buf, uint8_t len)
{
    return halM25PExxWrite(ZB_DATABASE_ADDR, buf, len) == 0;
}

bool zb_database_read(uint8_t *buf, uint8_t len)
{
    return halM25PExxRead(ZB_DATABASE_ADDR, buf, len) == 0;
}