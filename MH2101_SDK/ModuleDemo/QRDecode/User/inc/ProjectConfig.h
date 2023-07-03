#ifndef _PROJECTCONFIG_H_
#define _PROJECTCONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*单buff解码最小内存*/
#define IMAGE30W_SINGLE_BUFF_MIN_SIZE          (250 * 1024)
#define IMAGE8W_SINGLE_BUFF_MIN_SIZE           (125 * 1024)
/*双buff解码最小内存*/
#define IMAGE30W_DOUBLE_BUFF_MIN_SIZE          (460 * 1024)
#define IMAGE8W_DOUBLE_BUFF_MIN_SIZE           (200 * 1024)
/*解码buff大小*/
#define DECODE_BUFF_SIZE              (IMAGE30W_SINGLE_BUFF_MIN_SIZE)

/*蜂鸣器管脚设置*/
#define BEEP_GPIO                  GPIOC
#define BEEP_GOIO_PIN              GPIO_Pin_9
/*摄像头XCK管脚设置*/
#define CAM_XCK_GPIO               GPIOA
#define CAM_XCK_GPIO_PIN           GPIO_Pin_5
#define CAM_XCK_TIM                TIM_5
/*摄像头I2C管脚设置*/
#define SI2C_PORT                   GPIOB
#define SI2C_SCL_PIN                GPIO_Pin_0
#define SI2C_SDA_PIN                GPIO_Pin_1
#define SI2C_GPIO_REMAP             GPIO_Remap_0
/*摄像头PWDN管脚定义*/
#define CAM_PWDN_GPIO               GPIOC
#define CAM_PWDN_GOIO_PIN           GPIO_Pin_6
/*摄像头RST管脚定义*/
#define CAM_RST_GPIO                GPIOC
#define CAM_RST_GOIO_PIN            GPIO_Pin_7

// VSYNC
#define DCMI_VSYNC_GPIO_PORT    	GPIOE
#define DCMI_VSYNC_GPIO_PIN     	GPIO_Pin_1
#define DCMI_VSYNC_AF				GPIO_Remap_3

// HSYNC
#define DCMI_HSYNC_GPIO_PORT    	GPIOE
#define DCMI_HSYNC_GPIO_PIN     	GPIO_Pin_2
#define DCMI_HSYNC_AF				GPIO_Remap_3

// PIXCLK
#define DCMI_PIXCLK_GPIO_PORT   	GPIOE
#define DCMI_PIXCLK_GPIO_PIN    	GPIO_Pin_3
#define DCMI_PIXCLK_AF				GPIO_Remap_3

// DATA 0 ~ 7
#define DCMI_D0_GPIO_PORT       	GPIOD
#define DCMI_D0_GPIO_PIN        	GPIO_Pin_1
#define DCMI_D0_AF					GPIO_Remap_3
#define DCMI_D1_GPIO_PORT       	GPIOD
#define DCMI_D1_GPIO_PIN        	GPIO_Pin_2
#define DCMI_D1_AF					GPIO_Remap_3
#define DCMI_D2_GPIO_PORT       	GPIOD
#define DCMI_D2_GPIO_PIN        	GPIO_Pin_3
#define DCMI_D2_AF					GPIO_Remap_3
#define DCMI_D3_GPIO_PORT        	GPIOD
#define DCMI_D3_GPIO_PIN         	GPIO_Pin_8
#define DCMI_D3_AF					GPIO_Remap_3
#define DCMI_D4_GPIO_PORT        	GPIOD
#define DCMI_D4_GPIO_PIN         	GPIO_Pin_9
#define DCMI_D4_AF					GPIO_Remap_3
#define DCMI_D5_GPIO_PORT        	GPIOD
#define DCMI_D5_GPIO_PIN         	GPIO_Pin_10
#define DCMI_D5_AF					GPIO_Remap_3
#define DCMI_D6_GPIO_PORT        	GPIOD
#define DCMI_D6_GPIO_PIN         	GPIO_Pin_11
#define DCMI_D6_AF					GPIO_Remap_3
#define DCMI_D7_GPIO_PORT       	GPIOE
#define DCMI_D7_GPIO_PIN        	GPIO_Pin_0
#define DCMI_D7_AF					GPIO_Remap_3

/*摄像头模拟任务处理用时钟*/
#define CAM_TASK_TIMER              TIM_6
#define CAM_TASK_TIMER_IRQ          TIM0_6_IRQn
#define CAM_TASK_TIMER_IRQHANDLER   TIM0_6_IRQHandler

/*摄像头模拟VS，HS管脚*/
#define CAM_VIRTUAL_VS_GPIO         GPIOE
#define CAM_VIRTUAL_VS_PIN          GPIO_Pin_0

#define CAM_VIRTUAL_HS_GPIO         GPIOD
#define CAM_VIRTUAL_HS_PIN          GPIO_Pin_11

/*摄像头模拟pclk管脚*/
#define CAM_VIRTUAL_PCLK_GPIO       GPIOD
#define CAM_VIRTUAL_PCLK_PIN        GPIO_Pin_10


/**
 *单buff解码Demo
 *
 */
 void SingleBuffDecodeDemo(void);
  /**
  * 双buff解码demo
  *
  */
 void DoubleBuffDecodeDemo(void);

#ifdef __cplusplus
}
#endif

#endif
