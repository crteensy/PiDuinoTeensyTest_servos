#include <i2c_t3.h>

#include <PulsePosition.h>

static const uint8_t slaveAddress = 0x20;

static const uint8_t numChannels = 8;
typedef uint16_t servoValueType;
static servoValueType slaveReceiveBuffer[numChannels];

PulsePositionOutput ppmOut(RISING);

void rxFun(size_t len)
{
  if (len == sizeof(servoValueType)*numChannels)
  {
    for (size_t i = 0; i < numChannels; i++)
    {
      uint16_t val = (((Wire.readByte() & 0xFF) << 8) | (Wire.readByte() & 0xFF));
      slaveReceiveBuffer[i] = val;
      ppmOut.write(i+1, 1000. + 1000.*val/65535.);
    }
    return;
  }

  if (len == (1 + sizeof(servoValueType)))
  {
    uint8_t channel = Wire.readByte();
    if (channel >= numChannels)
    {
      // error: channel doesn't exist
      return;
    }
    uint16_t val = (((Wire.readByte() & 0xFF) << 8) | (Wire.readByte() & 0xFF));
    slaveReceiveBuffer[channel] = val;
    ppmOut.write(channel+1, 1000. + 1000.*val/65535.);
  }
}

void setup()
{
  Wire.begin(I2C_SLAVE, slaveAddress, I2C_PINS_16_17, I2C_PULLUP_EXT, I2C_RATE_400);
  Wire.onReceive(rxFun);

  ppmOut.begin(5, 6);  // pin 5: tx, pin 6: frame
  for (uint8_t i = 1; i < 9; i++)
  {
    ppmOut.write(i, 1500.);
  }
}

void loop()
{
}

