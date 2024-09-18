#pragma once

#include <Arduino.h>

/**
 * Class tLed
 * This class is used to control an LED connected to an Arduino board.
 * It supports turning the LED on, off, and blinking it at a specified interval.
 */
class tLed {
public:
  // Enumeration to define the state of the LED.
  enum LedState { OFF, ON, BLINK };

  /**
    * Default constructor.
    * Initializes the LED with an invalid pin number.
    */
  tLed() : _pin(-1) {}

  /**
    * Constructor with pin and onLevel parameters.
    * @param pin The GPIO pin number connected to the LED.
    * @param onLevel The logic level that turns the LED on (HIGH or LOW).
    */
  tLed(int pin, int onLevel = HIGH) {
    if (pin > 0) {
      _pin = pin;
      _onLevel = (onLevel == LOW) ? LOW : HIGH;

      pinMode(pin, OUTPUT);
      digitalWrite(pin, (_onLevel == HIGH) ? LOW : HIGH);
    }
  }

  /**
    * Sets the state of the LED with a specified blink interval.
    * @param state The desired state of the LED (OFF, ON, BLINK).
    * @param blinkIntervalMs The interval in milliseconds for the LED to blink if the state is BLINK.
    */
  void setState(LedState state, uint16_t blinkIntervalMs) {
    _setLedState(state, blinkIntervalMs);
  }

  /**
    * Sets the state of the LED using the previously set blink interval.
    * @param state The desired state of the LED (OFF, ON, BLINK).
    */
  void setState(LedState state) {
    _setLedState(state, _blinkIntervalMs);
  }

  /**
    * Template function to set the state of the LED with a type-flexible state and interval.
    * @param state The desired state of the LED (0 for OFF, 1 for ON, 2 for BLINK).
    * @param blinkIntervalMs The interval in milliseconds for the LED to blink.
    */
  template <typename T, typename M>
  void setState(T state, M blinkIntervalMs) {
    uint8_t u8State = static_cast<T>(state);
    LedState tmpState = (u8State == 1) ? (ON) : ((u8State == 2) ? BLINK : OFF);
    uint16_t blinkMs = static_cast<uint16_t>(blinkIntervalMs);

    setState(tmpState, blinkMs);
  }

  /**
    * Template function to set the state of the LED using the previously set blink interval.
    * @param state The desired state of the LED (0 for OFF, 1 for ON).
    */
  template <typename T>
  void setState(T state) {
    uint8_t u8State = static_cast<T>(state);
    LedState tmpState = (u8State == 1) ? (ON) : ((u8State == 2) ? BLINK : OFF);

    setState(tmpState);
  }

  /**
    * Turns the LED on using the last known blink interval.
    */
  void on() {
    setState(LedState::ON);
  }

  /**
    * Turns the LED off.
    */
  void off() {
    setState(LedState::OFF);
  }

  /**
    * Sets the LED to blink at a specified interval.
    * @param ms The interval in milliseconds for the LED to blink.
    */
  template <typename M>
  void blink(M ms) {
    uint16_t blinkMs = static_cast<uint16_t>(ms);

    setState(LedState::BLINK, blinkMs);
  }

  /**
    * Updates the state of the LED when it is set to blink.
    * This function should be called periodically to change the state of the LED based on the blink interval.
    */
  void update() {
    if (_pin <= 0) {
      return;
    }

    if (_ledState == BLINK && millis() >= _lastLedBlinkMs + _blinkIntervalMs) {
      _lastLedBlinkMs = millis();
      _nowLevel = (_nowLevel == LOW) ? HIGH : LOW;
      digitalWrite(_pin, _nowLevel);
    }
  }

  /**
    * Retrieves the current state of the LED.
    * @return The current state of the LED (OFF, ON, BLINK).
    */
  LedState getState() {
    return _ledState;
  }

  /**
    * Retrieves the current state of the LED.
    * @return The current state of the LED (0: OFF, 1: ON, 2: BLINK).
    */
  template <typename T>
  T getState() {
    uint8_t tmpState = (_ledState == ON) ? (1) : ((_ledState == BLINK) ? (2) : (0));
    T state = static_cast<T>(tmpState);

    return state;
  }

protected:
  int _pin;                  // GPIO pin number connected to the LED
  int _onLevel;              // Logic level that turns the LED on
  uint16_t _blinkIntervalMs; // Blink interval in milliseconds
  int _nowLevel;             // Current logic level of the LED
  unsigned long _lastLedBlinkMs; // Timestamp of the last LED blink

  void _setLedState(LedState state, uint16_t blinkIntervalMs) {
    int outLevel = 0;

    if (_pin <= 0) {
      return;
    }

    if (_ledState == state && _blinkIntervalMs == blinkIntervalMs) {
      return;
    }

    _ledState = state;

    switch (state) {
      case (BLINK) : {
        _blinkIntervalMs = blinkIntervalMs;

        outLevel = (_onLevel == HIGH) ? (HIGH) : (LOW);
        _nowLevel = outLevel;

        _lastLedBlinkMs = millis();

        digitalWrite(_pin, outLevel);
      } break;
      case (ON) : {
        outLevel = (_onLevel == HIGH) ? (HIGH) : (LOW);
        digitalWrite(_pin, outLevel);
      } break;
      default : { // off
        outLevel = (_onLevel == HIGH) ? (LOW) : (HIGH);
        digitalWrite(_pin, outLevel);
      } break;
    }

    return;
  }

private:
  LedState _ledState;        // Current state of the LED
};