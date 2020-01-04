#include "NES.h"

using namespace std;

NES::NES()
{
  m_bus = make_unique<Bus>();
  m_cpu = make_unique<Cpu>();
}
