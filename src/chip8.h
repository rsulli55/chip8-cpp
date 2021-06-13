#include <cstdint>
class Chip8 {

    public:
        std::uint8_t V0() { return V0_;}
        std::uint8_t V1() { return V1_;}
        std::uint8_t V2() { return V2_;}
        std::uint8_t V3() { return V3_;}
        std::uint8_t V4() { return V4_;}
        std::uint8_t V5() { return V5_;}
        std::uint8_t V6() { return V6_;}
        std::uint8_t V7() { return V7_;}
        std::uint8_t V8() { return V8_;}
        std::uint8_t V9() { return V9_;}
        std::uint8_t VA() { return VA_;}
        std::uint8_t VB() { return VB_;}
        std::uint8_t VC() { return VC_;}
        std::uint8_t VD() { return VD_;}
        std::uint8_t VE() { return VE_;}
        std::uint8_t VF() { return VF_;}

        std::uint16_t pc() { return pc_;}

    private:
    // registers
    std::uint16_t pc_ = 0x200;
    std::uint16_t I_ = 0x0;
    // general purpose registers
    std::uint8_t V0_ = 0x0;
    std::uint8_t V1_ = 0x0;
    std::uint8_t V2_ = 0x0;
    std::uint8_t V3_ = 0x0;
    std::uint8_t V4_ = 0x0;
    std::uint8_t V5_ = 0x0;
    std::uint8_t V6_ = 0x0;
    std::uint8_t V7_ = 0x0;
    std::uint8_t V8_ = 0x0;
    std::uint8_t V9_ = 0x0;
    std::uint8_t VA_ = 0x0;
    std::uint8_t VB_ = 0x0;
    std::uint8_t VC_ = 0x0;
    std::uint8_t VD_ = 0x0;
    std::uint8_t VE_ = 0x0;
    std::uint8_t VF_ = 0x0;
};
