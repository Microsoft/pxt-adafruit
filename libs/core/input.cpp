#include "pxt.h"
#include "DeviceSystemTimer.h"
#include "LIS3DH.h"

namespace pxt {
    
class Input {

  public:
    DeviceI2C i2c;
    DevicePin int1;
    LIS3DH acc;

    DevicePin temperature;
    AnalogSensor thermometer;

    DevicePin light;

    static Input *_instance;
    Input();
};

Input::Input()
    : // accelerometer
      i2c(PIN_ACCELEROMETER_SDA, PIN_ACCELEROMETER_SCL),
      INIT_PIN(int1, PIN_ACCELEROMETER_INT), //
      acc(i2c, int1),
      // temp.
      INIT_PIN(temperature, PIN_TEMPERATURE),
      thermometer(io.temperature, DEVICE_ID_THERMOMETER, 25, 10000, 3380, 10000, 273.5), //
      INIT_PIN(light, PIN_PIN_LIGHT) {}

Input *inp() {
    if (!Input::_instance)
        Input::_instance = new Input();
    return Input::_instance;
}
}

enum class Dimension {
    //% block=x
    X = 0,
    //% block=y
    Y = 1,
    //% block=z
    Z = 2,
    //% block=strength
    Strength = 3,
};

enum class Rotation {
    //% block=pitch
    Pitch = 0,
    //% block=roll
    Roll = 1,
};

enum class AcceleratorRange {
    /**
     * The accelerator measures forces up to 1 gravity
     */
    //%  block="1g"
    OneG = 1,
    /**
     * The accelerator measures forces up to 2 gravity
     */
    //%  block="2g"
    TwoG = 2,
    /**
     * The accelerator measures forces up to 4 gravity
     */
    //% block="4g"
    FourG = 4,
    /**
     * The accelerator measures forces up to 8 gravity
     */
    //% block="8g"
    EightG = 8
};

enum class Gesture {
    /**
     * Raised when shaken
     */
    //% block=shake
    Shake = ACCELEROMETER_EVT_SHAKE,
    /**
     * Raised when the device tilts up
     */
    //% block="tilt up"
    TiltUp = ACCELEROMETER_EVT_TILT_UP,
    /**
     * Raised when the device tilts down
     */
    //% block="tilt down"
    TiltDown = ACCELEROMETER_EVT_TILT_DOWN,
    /**
     * Raised when the screen is pointing left
     */
    //% block="tilt left"
    TiltLeft = ACCELEROMETER_EVT_TILT_LEFT,
    /**
     * Raised when the screen is pointing right
     */
    //% block="tilt right"
    TiltRight = ACCELEROMETER_EVT_TILT_RIGHT,
    /**
     * Raised when the screen faces up
     */
    //% block="face up"
    FaceUp = ACCELEROMETER_EVT_FACE_UP,
    /**
     * Raised when the screen is pointing up and the board is horizontal
     */
    //% block="face down"
    FaceDown = ACCELEROMETER_EVT_FACE_DOWN,
    /**
     * Raised when the board is falling!
     */
    //% block="free fall"
    FreeFall = ACCELEROMETER_EVT_FREEFALL,
    /**
    * Raised when a 3G shock is detected
    */
    //% block="3g"
    ThreeG = ACCELEROMETER_EVT_3G,
    /**
    * Raised when a 6G shock is detected
    */
    //% block="6g"
    SixG = ACCELEROMETER_EVT_6G,
    /**
    * Raised when a 8G shock is detected
    */
    //% block="8g"
    EightG = ACCELEROMETER_EVT_8G
};

//% noRefCounting fixedInstances
namespace ButtonMethods {
/**
 * Do something when a button (``A``, ``B`` or both ``A+B``) is pressed
 * @param button the button that needs to be pressed
 * @param body code to run when event is raised
 */
//% help=input/on-button-pressed weight=85 blockGap=8
//% blockId=device_button_event block="on button|%NAME|pressed"
//% parts="buttonpair"
//% blockNamespace=input
void onPressed(Button button, Action body) {
    registerWithDal(button->id, DEVICE_BUTTON_EVT_CLICK, body);
}

/**
 * Get the button state (pressed or not) for ``A`` and ``B``.
 * @param button the button to query the request, eg: Button.A
 */
//% help=input/button-is-pressed weight=60
//% block="button|%NAME|is pressed"
//% blockId=device_get_button2
//% icon="\uf192" blockGap=8
//% parts="buttonpair"
//% blockNamespace=input
bool isPressed(Button button) {
    return button->isPressed();
}
}

//% color="#FB48C7" weight=99 icon="\uf192"
namespace input {
/**
 * Do something when when a gesture is done (like shaking the micro:bit).
 * @param gesture the type of gesture to track, eg: Gesture.Shake
 * @param body code to run when gesture is raised
 */
//% help=input/on-gesture weight=84 blockGap=8
//% blockId=device_gesture_event block="on |%NAME"
//% parts="accelerometer"
void onGesture(Accelerometer accelerometer, Gesture gesture, Action body) {
    auto acc = &inp()->acc;
    int gi = (int)gesture;
    if (gi == ACCELEROMETER_EVT_3G && acc->getRange() < 3)
        acc->setRange(4);
    else if ((gi == ACCELEROMETER_EVT_6G || gi == ACCELEROMETER_EVT_8G) && acc->getRange() < 6)
        acc->setRange(8);
    registerWithDal(DEVICE_ID_GESTURE, gi, body);
}

/**
 * Get the acceleration value in milli-gravitys (when the board is laying flat with the screen up,
 * x=0, y=0 and z=-1024)
 * @param dimension TODO
 */
//% help=input/acceleration weight=58
//% blockId=device_acceleration block="acceleration (mg)|%NAME" blockGap=8
//% parts="accelerometer"
int acceleration(Dimension dimension) {
    switch (dimension) {
    case Dimension::X:
        return inp()->acc.getX();
    case Dimension::Y:
        return inp()->acc.getY();
    case Dimension::Z:
        return inp()->acc.getZ();
    case Dimension::Strength:
        return (int)sqrtf(inp()->acc.instantaneousAccelerationSquared());
    }
    return 0;
}

/**
 * Reads the light level applied to the LED screen in a range from ``0`` (dark) to ``255`` bright.
 */
//% help=input/light-level weight=57
//% blockId=device_get_light_level block="light level" blockGap=8
//% parts="ledmatrix"
int lightLevel() {
    // TODO fix up the range
    return inp()->light.getAnalogValue();
}

/**
 * Gets the temperature in Celsius degrees (°C).
 */
//% weight=55
//% help=input/temperature
//% blockId=device_temperature block="temperature (°C)" blockGap=8
//% parts="thermometer"
int temperature() {
    return inp()->thermometer.getTemperature();
}

/**
 * The pitch or roll of the device, rotation along the ``x-axis`` or ``y-axis``, in degrees.
 * @param kind TODO
 */
//% help=input/rotation weight=52
//% blockId=device_get_rotation block="rotation (°)|%NAME" blockGap=8
//% parts="accelerometer" advanced=true
int rotation(Rotation kind) {
    switch (kind) {
    case Rotation::Pitch:
        return inp()->acc.getPitch();
    case Rotation::Roll:
        return inp()->acc.getRoll();
    }
    return 0;
}

/**
 * Sets the accelerometer sample range in gravities.
 * @param range a value describe the maximum strengh of acceleration measured
 */
//% help=input/set-accelerometer-range
//% blockId=device_set_accelerometer_range block="set accelerometer|range %range"
//% weight=5
//% parts="accelerometer"
//% advanced=true
void setAccelerometerRange(AcceleratorRange range) {
    inp()->acc.setRange((int)range);
}

/**
  * Gets the number of milliseconds elapsed since power on.
  */
//% help=input/running-time weight=50
//% blockId=device_get_running_time block="running time (ms)"
//% advanced=true
int runningTime() {
    return system_timer_current_time();
}

}
