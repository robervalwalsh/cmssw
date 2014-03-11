#ifndef DATAFORMATS_HCALDETID_HCALDETID_H
#define DATAFORMATS_HCALDETID_HCALDETID_H 1

#include <iosfwd>
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/ForwardDetId/interface/ForwardSubdetector.h"


class BHMdetId : public DetId {
public:
  enum { Subdet=BHM};
  /** Create a null cellid*/
  BHMdetId();
  /** Create cellid from raw id (0=invalid tower id) */
  BHMdetId(uint32_t rawid); 
  /** Constructor from a generic cell id */
  BHMdetId(const DetId& id);
  //** Constructor from subedtector, side, and station
  BHMdetId(ForwardSubdetector subdet, int zside, int station);
  /** Assignment from a generic cell id */
  BHMdetId& operator=(const DetId& id);
  /** Comparison operator */   //temporarily on hold
  //bool operator==(DetId id) const;
  //bool operator!=(DetId id) const;
  //bool operator<(DetId id) const;

  /// get the subdetector
  ForwardSubdetector subdet() const { return BHM; }
  /// get the z-side of the cell (1/-1)
  int zside() const{ return (id_&0x80000)?(1):(-1); };
  /// get the station
  int station() const { return id_&0x3FF; };

  static const BHMdetId Undefined;

};

std::ostream& operator<<(std::ostream&,const BHMdetId& id);

#endif
