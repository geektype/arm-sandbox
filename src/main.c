#include <stdint.h>

#define NULL 0x0

#define FDT_START 0x40000000
#define FDT_MAX_SIZE 0x10000000
#define FDT_MAGIC 0xd00dfeed

#define FDT_HEADER_MAGIC_OFFSET 0x0
#define FDT_HEADER_TOTALSIZE_OFFSET 0x1
#define FDT_HEADER_STRUCT_OFFSET 0x2
#define FDT_HEADER_STRINGS_OFFSET 0x3
#define FDT_HEADER_MEM_OFFSET 0x4

#define FDT_SUCCESS 0;
#define FDT_ERR_INVALID_MAGIC 1;
#define FDT_ERR_TOO_BIG 2;

struct fdt_header {
  uint32_t totalsize;
  uint32_t *struct_block;
  uint32_t *string_block;
  uint32_t *mem_block;
  uint32_t version;
  uint32_t last_comp_version;
  uint32_t boot_cpuid_phys;
  uint32_t size_dt_strings;
  uint32_t size_dt_struct;
};

uint32_t swap_endian(uint32_t value) {
  return ((value & 0xFF000000) >> 24) | ((value & 0x00FF0000) >> 8) |
         ((value & 0x0000FF00) << 8) | ((value & 0x000000FF) << 24);
}

int main(void) {
  struct fdt_header header;

  uint32_t *fdt_start = (uint32_t *)FDT_START;
  uint32_t magic = swap_endian(fdt_start[FDT_HEADER_MAGIC_OFFSET]);
  if (magic != FDT_MAGIC)
    return FDT_ERR_INVALID_MAGIC;

  header.totalsize = swap_endian(fdt_start[FDT_HEADER_TOTALSIZE_OFFSET]);
  if (header.totalsize > FDT_MAX_SIZE)
    return FDT_ERR_TOO_BIG;

  uint32_t struct_offset = swap_endian(fdt_start[FDT_HEADER_STRUCT_OFFSET]);
  uint32_t string_offset = swap_endian(fdt_start[FDT_HEADER_STRINGS_OFFSET]);
  uint32_t mem_offset = swap_endian(fdt_start[FDT_HEADER_MEM_OFFSET]);

  header.struct_block = (uint32_t *)((uint8_t *)fdt_start + struct_offset);
  header.string_block = (uint32_t *)((uint8_t *)fdt_start + string_offset);
  header.mem_block = (uint32_t *)((uint8_t *)fdt_start + mem_offset);

  return 0;
}
