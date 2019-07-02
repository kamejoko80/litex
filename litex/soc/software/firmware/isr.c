#include <generated/csr.h>
#include <irq.h>
#include <uart.h>
#include <stdio.h>

#ifdef __rocket__
void plic_init(void);
void plic_init(void)
{
	int i;

	// priorities for interrupt pins 1..4
	for (i = 1; i <= 4; i++)
		csr_writel(1, PLIC_BASE + 4*i);
	// enable interrupt pins 1..4
	csr_writel(0xf << 1, PLIC_ENABLED);
	// set priority threshold to 0 (any priority > 0 triggers interrupt)
	csr_writel(0, PLIC_THRSHLD);
}

void isr(void);
void isr(void)
{
	unsigned int claim;

	while ((claim = csr_readl(PLIC_CLAIM))) {
		switch (claim - 1) {
		case UART_INTERRUPT:
			uart_isr();
			break;
		default:
			printf("## PLIC: Unhandled claim: %d\n", claim);
			printf("# plic_enabled:    %08x\n", irq_getmask());
			printf("# plic_pending:    %08x\n", irq_pending());
			printf("# mepc:    %016lx\n", csrr(mepc));
			printf("# mcause:  %016lx\n", csrr(mcause));
			printf("# mtval:   %016lx\n", csrr(mtval));
			printf("# mie:     %016lx\n", csrr(mie));
			printf("# mip:     %016lx\n", csrr(mip));
			printf("###########################\n\n");
			break;
		}
		csr_writel(claim, PLIC_CLAIM);
	}
}
#else
    
#ifdef GPIO_ISR_INTERRUPT
void gpio_isr_interrupt(void); 
void gpio_isr_interrupt(void) 
{
  unsigned int status;

  status = gpio_isr_ev_pending_read(); // you don't need to do this if you just have one interrupt source
  
  // gpio_isr_ev_pending_write(1);     // You'd do this if you just had one interrupt

  if( status & 1 ) {
    printf("Hi! I got interrupt 1\n");
    gpio_isr_ev_pending_write(1);    // clear the interrupt so it doesn't keep on firing and wedge the CPU
  } else if( status & 2 ) {
    printf("Hi! I got interrupt 2\n");
    gpio_isr_ev_pending_write(2);
  }

  gpio_isr_ev_enable_write(1);  // re-enable the event handler so we can catch the interrupt again
}
#endif

#ifdef CAN_CTRL_INTERRUPT
void can_ctrl_interrupt(void);
void can_ctrl_interrupt(void)
{
  unsigned int status;

  status = can_ctrl_ev_pending_read(); // you don't need to do this if you just have one interrupt source

  if( status & 1 ) 
  {
    printf("Can ctrl interrupt\r\n");
    can_ctrl_ev_pending_write(1);     // clear the interrupt so it doesn't keep on firing and wedge the CPU
  } 

  can_ctrl_ev_enable_write(1);        // re-enable the event handler so we can catch the interrupt again    
}
#endif

void isr(void);
void isr(void)
{
    unsigned int irqs;

    irqs = irq_pending() & irq_getmask();

    if(irqs & (1 << UART_INTERRUPT))
        uart_isr();
    
#ifdef CAN_CTRL_INTERRUPT
    if(irqs & (1 << CAN_CTRL_INTERRUPT))
        can_ctrl_interrupt();    
#endif

#ifdef GPIO_ISR_INTERRUPT
    if(irqs & (1 << GPIO_ISR_INTERRUPT))
        gpio_isr_interrupt();
#endif

}
#endif
