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
  `f052` INT NOT NULL,
  `f052B` INT NOT NULL,
  `f077` INT NOT NULL,
  `f087` INT NOT NULL,
  `f90007I` VARCHAR(255) NOT NULL,
  `t085` DOUBLE NOT NULL,
  PRIMARY KEY (`ID`),
  UNIQUE INDEX `ID_UNIQUE` (`ID` ASC) VISIBLE);

--------------------------------------------------------------------------------
OPCUA

EndPoint: opc.tcp://192.168.11.111:4880/Arburg
Name: host_computer
Password: 

ARBURG/ProductionControl/f052/f052-Value