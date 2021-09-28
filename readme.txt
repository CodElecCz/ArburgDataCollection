CREATE TABLE `reportdataimm` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `machine_id` varchar(45) NOT NULL,
  `datetime` datetime NOT NULL,
  `count` int(11) NOT NULL,
  `SetDescPrt` varchar(45) NOT NULL,
  `ActCntMld` int(11) NOT NULL,
  `SetDescMld` varchar(45) NOT NULL,
  `ActCntCyc` int(11) NOT NULL,
  `ActTimCyc` double NOT NULL,
  `ActStsMach` varchar(45) NOT NULL,
  PRIMARY KEY (`ID`),
  UNIQUE KEY `ID_UNIQUE` (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8
  
ODBC Data source: KSR
usr: KSR
pasw: ksr
schema: ksr
127.0.0.1

D:\Work\Development\Qt\EngelDataCollection\MACHINES\176798\E63_JOBS\ReportDataIMM.dat
D:\Work\Development\Qt\EngelDataCollection\MACHINES\180948\E63_JOBS\ReportDataIMM.dat
D:\Work\Development\Qt\EngelDataCollection\MACHINES\181793\E63_JOBS\ReportDataIMM.dat
D:\Work\Development\Qt\EngelDataCollection\MACHINES\183769\E63_JOBS\ReportDataIMM.dat
D:\Work\Development\Qt\EngelDataCollection\MACHINES\186120\E63_JOBS\ReportDataIMM.dat
D:\Work\Development\Qt\EngelDataCollection\MACHINES\186121\E63_JOBS\ReportDataIMM.dat
D:\Work\Development\Qt\EngelDataCollection\MACHINES\194365\E63_JOBS\ReportDataIMM.dat
D:\Work\Development\Qt\EngelDataCollection\MACHINES\198579\E63_JOBS\ReportDataIMM.dat
D:\Work\Development\Qt\EngelDataCollection\MACHINES\207499\E63_JOBS\ReportDataIMM.dat
D:\Work\Development\Qt\EngelDataCollection\MACHINES\219487\E63_JOBS\ReportDataIMM.dat

DATE,TIME,COUNT,SetDescPrt,ActCntMld,SetDescMld,ActCntCyc,ActTimCyc,ActStsMach
20210209,21:05:23,1,"445794/695576",2,"M-11175",6,35.23,"0A011"
20210209,21:06:25,2,"445794/695576",2,"M-11175",7,0.85,"0A011"
20210209,21:10:30,3,"445794/695576",2,"M-11175",8,1.81,"0A011"