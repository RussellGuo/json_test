``` mermaid

sequenceDiagram
    participant APP as App
    participant AS as Android语义层
    participant ADG as Android数据报层
    participant ADR as Android驱动层
    participant MDR as MCU驱动层层
    participant MDG as MCU数据报层
    participant MS  as MCU语义层
    participant SER as MCU服务（复数）
    APP->>AS: remote_call(req)，req是业务对象
    AS->> ADG: 建立通用req类型，放入req，生成数据报
    ADG->>ADR: 发送数据报。错误发生后可恢复
    ADR->>MDR: 驱动和硬件尽量保证传输无误
    MDR->>MDG: 解析数据报
    MDG->>MS: 按照req来解析
    MS->>SER: 按照精确的req类型去调用服务
    SER->>MS: 运行，然后返回对应的res对象
    MS->>MDG: 打包res对象到“总”res对象
    MDG->>MDR: c生成数据报
    MDR->>ADR: 发送数据报 

```

