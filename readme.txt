--------------------------------------------------------------------------------
ODBC 

Data source: KSR
usr: ksr
pasw: ksr
schema: ksr
127.0.0.1

DATE,TIME,f052,f052B,f077,f087,f90007I,t085

CREATE TABLE `ksr`.`arburg` (
  `ID` INT NOT NULL AUTO_INCREMENT,
  `datetime` DATETIME NOT NULL,
  `f052` VARCHAR(45) NOT NULL,
  `f052B` VARCHAR(45) NOT NULL,
  `f077` INT NOT NULL,
  `f087` INT NOT NULL,
  `f90007I` VARCHAR(255) NOT NULL,
  `t085` DOUBLE NOT NULL,
  `status` INT NOT NULL,
  PRIMARY KEY (`ID`),
  UNIQUE INDEX `ID_UNIQUE` (`ID` ASC) VISIBLE);

--------------------------------------------------------------------------------
OPCUA

EndPoint: opc.tcp://192.168.11.111:4880/Arburg
Name: host_computer
Password: 

Root/Objects/Arburg/ProductionControl/f052/Value        ns=2;i=412862
Root/Objects/Arburg/ProductionControl/f052B/Value       ns=2;i=412872
Root/Objects/Arburg/ProductionControl/f077/Value        ns=2;i=239002
Root/Objects/Arburg/ProductionControl/f087/Value        ns=2;i=239022
Root/Objects/Arburg/ProductionControl/f90007I/Value     ns=2;i=238922
Root/Objects/Arburg/ProductionControl/t085/Value        ns=2;i=416422


�
  `f052` VARCHAR(45),          - polo�ka
  `f052B` VARCHAR(45),         - popis polo�ky
  `f077` INT,                  - dobr� d�ly celkem
  `f087` INT,                  - �patn� d�ly celkem
  `f90007I` VARCHAR(255),      - info o zak�zce
  `t085` DOUBLE                - cycletime  
�

Tak�e s ka�dou zm�nou f077/f087 se do database zap�e:
11:04:25 29.09.2021, �1020245�, ��,2456, 16, �MATERIAL:ALCOM PA66 910/1 SLDS,084189  TOPENI FORMY:4x300`C DOBA SUSENI 4h80`C  M-11160�, 29.84
11:05:05 29.09.2021, �1020245�, ��,2457, 16, �MATERIAL:ALCOM PA66 910/1 SLDS,084189  TOPENI FORMY:4x300`C DOBA SUSENI 4h80`C  M-11160�, 29.84
11:06:05 29.09.2021, �1020245�, ��,2457, 17, �MATERIAL:ALCOM PA66 910/1 SLDS,084189  TOPENI FORMY:4x300`C DOBA SUSENI 4h80`C  M-11160�, 29.84
�
