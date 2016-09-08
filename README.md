# Simple IoT sensor DS18B20 to mqtt
```
  ┌───────────┐     
  │           │     
  │           │     
  │  DS18B20  │     
  │           │     
  │           │     
  │           │     
  └───────────┘     
    ┌─┐┌─┐┌─┐       
    │1││2││3│       
    └─┘└─┘└─┘       
     │  │  │        
   ┌─┘  │  └────┐   
   │    │       │   
┌────┐  │   ┌──────┐
│gnd │  │   │gpio14│
└────┘  │   └──────┘
     ┌─────┐        
     │gpio2│        
     └─────┘        

```