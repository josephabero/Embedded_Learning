# GPIO Driver for SJTwo Board



### GPIO Memory Map Lookup
```C
volatile const LPC_GPIO_TypeDef *lpc_gpio[] = {
	LPC_GPIO0, 
	LPC_GPIO1,
	LPC_GPIO2,
	LPC_GPIO3,
	LPC_GPIO4,
	LPC_GPIO5
};

```

```C
static LPC_GPIO_TypeDef *gpio_get_struct(gpio_s gpio) { 
	return (LPC_GPIO_TypeDef *)lpc_gpio[gpio.port]; 
}
```

### GPIO Structure
```C
typedef struct {
  uint8_t port;
  uint8_t pin;
} gpio_s;
```

### Example Driver Functions
```C
void gpio_set_as_output(gpio_s gpio) {
  LPC_GPIO_TypeDef *port = gpio_get_struct(gpio);
  port->DIR |= (1 << gpio.pin);
}
```

```C
void gpio_set_high(gpio_s gpio) {
  LPC_GPIO_TypeDef *port = gpio_get_struct(gpio);
  port->PIN |= (1 << gpio.pin);
}
```