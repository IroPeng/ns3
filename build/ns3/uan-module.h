
#ifdef NS3_MODULE_COMPILATION
# error "Do not include ns3 module aggregator headers from other modules; these are meant only for end user scripts."
#endif

#ifndef NS3_MODULE_UAN
    

// Module headers:
#include "acoustic-modem-energy-model-helper.h"
#include "acoustic-modem-energy-model.h"
#include "uan-address.h"
#include "uan-channel.h"
#include "uan-header-common.h"
#include "uan-header-rc.h"
#include "uan-helper.h"
#include "uan-l3-protocol-helper.h"
#include "uan-l3-protocol.h"
#include "uan-mac-aloha.h"
#include "uan-mac-cw.h"
#include "uan-mac-rc-gw.h"
#include "uan-mac-rc.h"
#include "uan-mac.h"
#include "uan-net-device.h"
#include "uan-noise-model-default.h"
#include "uan-noise-model.h"
#include "uan-phy-dual.h"
#include "uan-phy-gen.h"
#include "uan-phy.h"
#include "uan-prop-model-ideal.h"
#include "uan-prop-model-thorp.h"
#include "uan-prop-model.h"
#include "uan-routing-dbr.h"
#include "uan-routing-dpr.h"
#include "uan-routing-header-dbr.h"
#include "uan-routing-header-dpr.h"
#include "uan-routing-header.h"
#include "uan-routing-test.h"
#include "uan-transducer-hd.h"
#include "uan-transducer.h"
#include "uan-tx-mode.h"
#include "uan-upper-layer.h"
#endif
