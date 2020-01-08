#pragma once

#include "Mapper.h"

class Mapper_000 : public Mapper
{
public:
  Mapper_000(uint8_t prgBanks, uint8_t chrBanks);
  ~Mapper_000() {}

  // Inherited via Mapper
  virtual void MapReadByCPU(uint16_t addr, uint32_t & mapped_addr) override;
  virtual void MapWriteByCPU(uint16_t addr, uint32_t & mapped_addr) override;
  virtual void MapReadByPPU(uint16_t addr, uint32_t & mapped_addr) override;
  virtual void MapWriteByPPU(uint16_t addr, uint32_t & mapped_addr) override;
};

