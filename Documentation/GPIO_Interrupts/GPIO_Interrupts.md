# GPIO Interrupts for SJTwo Board

### Interrupts
#### What are Interrupts?
An `Interrupt` is an event that occurs when an expected signal happens. 

**Example**: Button is pressed, then a LED turns on. 

Ok, so how do we set up an Interrupt? 



### Interrupt Service Routine (ISR)
An `Interrupt Service Routine (ISR)` is the code that runs when our buttons is PRESSED! 

**Example**: Tell the LED to turn on.
```C
void button_ISR() {
  gpio_set_high(led); // Turn on LED
}
```


### Interrupt Vector Table
```C
const function__void_f interrupt_vector_table[] = {
	.
	.
	lpc_peripheral__interrupt_dispatcher, // 53 LCD
    gpio_interrupt_dispatcher,            // 54 GPIO Interrupt
    lpc_peripheral__interrupt_dispatcher, // 55 PWM 0
    .
    .
};
```

### Attach Custom Interrupt Dispatcher
```C
void gpio_attach_interrupt(
	gpio_s gpio, 
	gpio_interrupt_e interrupt_type, 
	function_pointer_t callback
);
void gpio_interrupt_dispatcher(void);
```


### GPIO Interrupt Structure
```C
typedef enum {
  GPIO_INTR__FALLING_EDGE,
  GPIO_INTR__RISING_EDGE,
} gpio_interrupt_e;
```

### Function Pointers To ISR
```C
// 3 GPIO Ports, each with 32 Pins
// Stores ISR for each Pin when using `gpio_attach_interrupt`
typedef void (*function_pointer_t)(void);
static function_pointer_t gpio_callbacks_rising[3][32];
static function_pointer_t gpio_callbacks_falling[3][32];
```



