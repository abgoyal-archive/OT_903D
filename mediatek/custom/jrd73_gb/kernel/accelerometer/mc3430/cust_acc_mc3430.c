
#include <cust_acc.h>
#include <mach/mt6573_pll.h>
/*---------------------------------------------------------------------------*/
int mc3430_cust_acc_power(struct acc_hw *hw, unsigned int on, char* devname)
{
    if (hw->power_id == MT65XX_POWER_NONE)
        return 0;
    if (on)
        return hwPowerOn(hw->power_id, hw->power_vol, devname);
    else
        return hwPowerDown(hw->power_id, devname); 
}
/*---------------------------------------------------------------------------*/
static struct acc_hw mc3430_cust_acc_hw = {
    .i2c_num = 0,
    .direction = 7,
    .power_id = MT65XX_POWER_NONE,  /*!< LDO is not used */
    .power_vol= VOL_DEFAULT,        /*!< LDO is not used */
    .firlen = 0,                   /*!< don't enable low pass fileter */
    .power = mc3430_cust_acc_power,        
};
/*---------------------------------------------------------------------------*/
struct acc_hw* mc3430_get_cust_acc_hw(void) 
{
    return &mc3430_cust_acc_hw;
}
