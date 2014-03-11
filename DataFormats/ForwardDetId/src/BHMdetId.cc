#include "DataFormats/ForwardDetId/interface/BHMdetId.h"
#include "FWCore/Utilities/interface/Exception.h"
#include <ostream>

const BHMdetId BHMdetId::Undefined(ForwardEmpty,0,0);

BHMdetId::BHMdetId() : DetId() {
}

BHMdetId::BHMdetId(uint32_t rawid) : DetId(rawid) {
}

BHMdetId::BHMdetId(ForwardSubdetector subdet, int zside, int station) : DetId(Forward,subdet) {
  // (no checking at this point!)
  id_ |= (((zside>0) ? 0x1000000 : 0) | ((station&0x1F)<14) );
}

BHMdetId::BHMdetId(const DetId& gen) {
  if (!gen.null()) {
    ForwardSubdetector subdet=(ForwardSubdetector(gen.subdetId()));
    if (gen.det()!=Forward || (subdet!=BHM)) {
      throw cms::Exception("Invalid DetId") << "Cannot initialize BHMdetId from " << std::hex << gen.rawId() << std::dec; 
    }  
  }
  id_ = gen.rawId();
}

BHMdetId& BHMdetId::operator=(const DetId& gen) {
  if (!gen.null()) {
    ForwardSubdetector subdet=(ForwardSubdetector(gen.subdetId()));
    if (gen.det()!=Forward || (subdet!=BHM)) {
      throw cms::Exception("Invalid DetId") << "Cannot assign BHMdetId from " << std::hex << gen.rawId() << std::dec; 
    }  
  }
  id_ = gen.rawId();
  return (*this);
}

std::ostream& operator<<(std::ostream& s,const BHMdetId& id) {
  switch (id.subdet()) {
  case(HGCEE) : return s << "(HGCEE " << id.zside() << ',' << id.station()  << ')';
  default : return s << id.rawId();
  }
}
