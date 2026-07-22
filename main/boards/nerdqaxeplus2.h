#pragma once

#include "asic.h"
#include "bm1370.h"
#include "board.h"
#include "nerdqaxeplus.h"

class NerdQaxePlus2 : public NerdQaxePlus {
  private:
    bool m_hasRev7TPS546 = false;
    uint16_t m_rev7VoltageDomains = 2;

    void applyRev7Profile();
    void selectRev7BuckConverter();
    bool probeRev7Buck();

  public:
    NerdQaxePlus2();
    bool initBoard() override;
    bool initAsics() override;
    bool setVoltage(float core_voltage) override;
    float getTemperature(int index);
    void requestChipTemps() override;

    // Optional W5500 add-on (LAN). GPIO13 is already used as LDO_EN_PIN on
    // this board, so the reset line is moved to GPIO4 (verify against the
    // physical wiring before relying on this).
    bool hasEthernet() override {
        return true;
    }
    int getEthResetPin() override {
        return 4;
    }
};
