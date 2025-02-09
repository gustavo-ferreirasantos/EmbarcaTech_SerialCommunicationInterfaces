#include <stdio.h>           // Entrada e saída padrão (printf, scanf)  
#include "pico/stdlib.h"     // Biblioteca padrão do Raspberry Pi Pico  
#include "hardware/pio.h"    // Controle de periféricos PIO  
#include "hardware/clocks.h" // Configuração dos clocks do microcontrolador  

#include "pio_matrix.pio.h"  // Código PIO para controle de matriz  

#include "hardware/i2c.h"    // Comunicação I2C (sensores, displays)  
#include "inc/font.h"        // Fonte para exibição em displays  
#include "inc/ssd1306.h"     // Controle do display OLED SSD1306  


//Protótipos das funções:
void init();
uint32_t matrix_rgb(double b, double r, double g);
void desenho_pio(double *desenho, double r, double g, double b);
void button_callback(uint gpio, uint32_t events);
void numbers(int number);

// Definições de pinos e endereços I2C
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define I2C_ADDR 0x3C   // Endereço do dispositivo I2C

#define NUM_PIXELS 25   // Número de pixels na matriz de LEDs
#define OUT_PIN 7       // Pino de saída para a matriz de LEDs
#define BLUE_LED 12     // Pino do LED azul
#define GREEN_LED 11    // Pino do LED verde
#define BUTTON_A 5      // Pino do botão A
#define BUTTON_B 6      // Pino do botão B

// Variáveis globais:
static volatile uint32_t last_time = 0; // Armazena o tempo da última interrupção
char last_character; // Último caractere recebido
bool display_updated = true; // Indica se o display precisa ser atualizado

ssd1306_t ssd; // Estrutura de controle do display OLED
PIO pio; // Instância do PIO
uint sm; // Estado da máquina de estado PIO
uint32_t valor_led; // Valor RGB do LED


// Inicializa os leds e botões
void init(){
    stdio_init_all(); // Inicializa entrada e saída padrão
    
    // Inicializa os LEDs
    gpio_init(GREEN_LED);
    gpio_set_dir(GREEN_LED, GPIO_OUT);
    gpio_init(BLUE_LED);
    gpio_set_dir(BLUE_LED, GPIO_OUT);

    // Inicializa o botão A
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);

    // Inicializa o botão B
    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
}



// Converte valores de cor (RGB) para um formato de 32 bits
uint32_t matrix_rgb(double b, double r, double g){
    unsigned char R, G, B;
    R = r * 255;
    G = g * 255;
    B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

// Envia um desenho para a matriz de LEDs usando PIO
void desenho_pio(double *desenho, double r, double g, double b){
    for (int16_t i = 0; i < NUM_PIXELS; i++){
        valor_led = matrix_rgb(desenho[24 - i] * b, desenho[24 - i] * r, desenho[24 - i] * g);
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}



// Callback de interrupção para os botões
void button_callback(uint gpio, uint32_t events) {
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    // Implementação de debounce (ignora eventos em menos de 300ms)
    if (current_time - last_time > 300000) {
        last_time = current_time;
        if (gpio == BUTTON_A) {
            gpio_put(GREEN_LED, !gpio_get(GREEN_LED)); // Alterna LED verde
            printf("Botão A pressionado - LED Verde agora está %s.\n", gpio_get(GREEN_LED) ? "ligado" : "desligado");
        }
        else {
            gpio_put(BLUE_LED, !gpio_get(BLUE_LED)); // Alterna LED azul
            printf("Botão B pressionado - LED Azul agora está %s.\n", gpio_get(BLUE_LED) ? "ligado" : "desligado");
        }
        display_updated = true;
    }
}

// Função Principal
int main(){
    // Configuração dos leds e botões
    init();
    
    // Configuração da Matrix de leds
    pio = pio0;
    uint offset = pio_add_program(pio, &pio_matrix_program);
    sm = pio_claim_unused_sm(pio, true);
    pio_matrix_program_init(pio, sm, offset, OUT_PIN);

    // Configurações do I2C
    i2c_init(I2C_PORT, 100 * 1000); // Inicializa I2C0 a 100kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // SDA
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // SCL
    gpio_pull_up(I2C_SDA); // Habilita pull-up para SDA
    gpio_pull_up(I2C_SCL); // Habilita pull-up para SCL
    
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, I2C_ADDR, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd); // Configura o display
    ssd1306_send_data(&ssd); // Envia os dados para o display
    
    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
    
    uint8_t buffer[2];
    bool cor = true;

    // Habilita a interrupção na borda de descida para o botão A e B
    gpio_set_irq_enabled(BUTTON_A, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(BUTTON_B, GPIO_IRQ_EDGE_FALL, true);
    // Define a função de callback que será chamada quando uma interrupção ocorrer
    gpio_set_irq_callback(button_callback);
    irq_set_enabled(IO_IRQ_BANK0, true);


    while (true){
        int character = getchar_timeout_us(0);
        // verifica se recebeu algum dado via USB Serial
        if(character != PICO_ERROR_TIMEOUT){
            if(character >= '0' && character <= '9'){
                numbers(character - '0'); // Número exibido na matriz 5x5 WS2812
            }else{
                numbers(10); // Clear
            }
            last_character = (char)character;
            display_updated = true;
        }

        if(display_updated){
            cor = !cor;
            // Atualiza o conteúdo do display com animações
            ssd1306_fill(&ssd, !cor);
            ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor); // Desenha um retângulo
            ssd1306_draw_string(&ssd, "CHARACTER: ", 8, 10); // Desenha uma string
            ssd1306_draw_char(&ssd, last_character, 90, 10); // Desenha um caractere
            ssd1306_draw_string(&ssd, "GREEN LED: ", 8, 29); // Desenha uma string     
            ssd1306_draw_string(&ssd, gpio_get(GREEN_LED)? "ON": "OFF", 90, 29); // Desenha uma string
            ssd1306_draw_string(&ssd, "BLUE LED: ", 8, 48); // Desenha uma string
            ssd1306_draw_string(&ssd, gpio_get(BLUE_LED)? "ON": "OFF", 90, 48); // Desenha uma string
            ssd1306_send_data(&ssd); // Atualiza o display
            display_updated = false;
        }
    }
}

void numbers(int number){

    double zero[25] = {
        0.0, 1.0, 1.0, 1.0, 0.0,
        0.0, 1.0, 0.0, 1.0, 0.0,
        0.0, 1.0, 0.0, 1.0, 0.0,
        0.0, 1.0, 0.0, 1.0, 0.0,
        0.0, 1.0, 1.0, 1.0, 0.0};
    
    double one[25] = {
        0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 1.0, 0.0,
        1.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0, 0.0};

    double two[25] = {
        0.0, 1.0, 1.0, 1.0, 0.0,
        0.0, 1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 1.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0, 0.0,
        0.0, 1.0, 1.0, 1.0, 0.0};

    double three[25] = {
        0.0, 1.0, 1.0, 1.0, 0.0,
        0.0, 1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 1.0, 1.0, 0.0,
        0.0, 1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 1.0, 1.0, 0.0};

    double four[25] = {
        0.0, 1.0, 0.0, 1.0, 0.0,
        0.0, 1.0, 0.0, 1.0, 0.0,
        0.0, 1.0, 1.0, 1.0, 0.0,
        0.0, 1.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 1.0, 0.0};

    double five[25] = {
        0.0, 1.0, 1.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0, 0.0,
        0.0, 1.0, 1.0, 1.0, 0.0,
        0.0, 1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 1.0, 1.0, 0.0};
    
    double six[25] = {
        0.0, 1.0, 1.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0, 0.0,
        0.0, 1.0, 1.0, 1.0, 0.0,
        0.0, 1.0, 0.0, 1.0, 0.0,
        0.0, 1.0, 1.0, 1.0, 0.0};
    
    double seven[25] = {
        0.0, 1.0, 1.0, 1.0, 0.0,
        0.0, 1.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 1.0, 0.0,
        0.0, 1.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 1.0, 0.0};
   
    double eight[25] = {
        0.0, 1.0, 1.0, 1.0, 0.0,
        0.0, 1.0, 0.0, 1.0, 0.0,
        0.0, 1.0, 1.0, 1.0, 0.0,
        0.0, 1.0, 0.0, 1.0, 0.0,
        0.0, 1.0, 1.0, 1.0, 0.0};
   
    double nine[25] = {
        0.0, 1.0, 1.0, 1.0, 0.0,
        0.0, 1.0, 0.0, 1.0, 0.0,
        0.0, 1.0, 1.0, 1.0, 0.0,
        0.0, 1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 1.0, 1.0, 0.0};

    double clear[25] = {
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0};


    double* digits[11] = {zero, one, two, three, four, five, six, seven, eight, nine, clear};
    desenho_pio(digits[number], 0.0, 0.0, 1.0);
}
