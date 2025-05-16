# DMA Activator Documentation

## Signal Configuration Guide / 信号配置指南

### Activation Signal Definition / 激活信号定义
The activation signal is defined in the `pcileech_ft601.sv` file. The key components are:

激活信号定义在 `pcileech_ft601.sv` 文件中。主要组件包括：

1. Signal Registers / 信号寄存器 (Line 92-93):
```verilog
reg [31:0] SIGN;      // Stores the activation signal / 存储激活信号
reg DETECT;           // Detection flag / 检测标志
```

2. Detection Value / 检测值 (Line 98):
```verilog
else if (dout == 32'hDEADBEEF | FT601_DATA == 32'hDEADBEEF)
```

### How to Modify / 如何修改

1. To change the activation signal value / 修改激活信号值:
   - Locate line 98 in `pcileech_ft601.sv`
   - Replace `32'hDEADBEEF` with your desired 32-bit hex value
   - 在 `pcileech_ft601.sv` 第98行
   - 将 `32'hDEADBEEF` 替换为你想要的32位十六进制值

2. To modify signal behavior / 修改信号行为:
   - The signal detection logic is in the always block starting from line 95
   - The output control logic is around line 116
   - 信号检测逻辑在第95行开始的 always 块中
   - 输出控制逻辑在第116行附近

## Working Principle / 工作原理

1. Signal Detection / 信号检测:
   - The system continuously monitors both `dout` and `FT601_DATA` for the activation signal
   - When the predefined value (default: 0xDEADBEEF) is detected, `DETECT` flag is set
   - 系统持续监控 `dout` 和 `FT601_DATA` 的激活信号
   - 当检测到预定义值（默认：0xDEADBEEF）时，设置 `DETECT` 标志

2. Data Flow / 数据流:
   - Input data is processed through the FT601 interface
   - Data is byte-swapped during transfer (31:24 ↔ 7:0, 23:16 ↔ 15:8)
   - Output control is managed by state machine
   - 输入数据通过FT601接口处理
   - 数据在传输过程中进行字节交换（31:24 ↔ 7:0, 23:16 ↔ 15:8）
   - 输出控制由状态机管理

3. State Machine / 状态机:
   - IDLE: Waits for RX or TX signals
   - RX states: Handle data reception
   - TX states: Handle data transmission
   - Cooldown states: Manage timing between operations
   - IDLE：等待RX或TX信号
   - RX状态：处理数据接收
   - TX状态：处理数据发送
   - 冷却状态：管理操作之间的时序

## Important Notes / 重要说明

1. The system prioritizes RX operations over TX when both are available
   当RX和TX同时可用时，系统优先处理RX操作

2. Data queue management is handled automatically with cooldown periods
   数据队列管理自动处理，包含冷却期

3. The activation signal detection is synchronized with the clock
   激活信号检测与时钟同步

4. Modifications should maintain the timing and protocol requirements of the FT601 interface
   修改时应保持FT601接口的时序和协议要求







### Advance
1. 添加了一个新的寄存器 LOCKED，用于记录是否已经锁定状态

2. 在复位信号 rst 时，将 LOCKED 设置为 0（解锁状态）

3. 只有在未锁定状态（!LOCKED）下才会检测正确的信号（32'hDEADBEEF）

4. 如果在未锁定状态下收到错误的信号，就会将 LOCKED 设置为 1（锁定状态）
这样修改后的行为将是：
系统复位（rst=1）时，所有状态都会被清零，包括解除锁定
第一次收到信号时：
如果是正确的信号（32'hDEADBEEF），则激活 DETECT
如果是错误的信号，则进入锁定状态，之后即使收到正确信号也不会激活

5. 只有通过系统复位（板子断电重启）才能解除锁定状态
