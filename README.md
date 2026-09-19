  一个基于 STM32F103C8T6（标准外设库）的小健康监测项目，用 OLED 屏幕实时显示心率、血氧、体温，并带摔倒检测。
  
  传感器用的 MAX30102（心率血氧）、DS18B20（体温）、MPU6050（姿态），三个 I²C 器件挂同一条软件模拟 I²C总线（PB6/PB7），DS18B20 接 PA0，MAX30102 的中断脚接 PA7。

  编译用 Keil MDK，打开 Project.uvprojx 直接编译烧录就行，标准库已经放在仓库里了。

  心率血氧用的是 Maxim 官方的 maxim_heart_rate_and_oxygen_saturation 算法，摔倒检测就是看 MPU6050加速度矢量和相对初始值的变化量，超过阈值就报摔倒。上电后手指压在 MAX30102 上静置几秒，数值就稳定了。
