[CPS_BUAP_filemgr]
resourcegroup	= Disk Group
dependency	= FMS_CPF_SERVER
restart		= YES
affectgroup	= YES
desc		= CPS Backup/Restore File Manager
service		= CPS_BUAP_filemgr
stoptime	= 2000
options		= 

[CPS_BUAP_loader]
resourcegroup	= Disk Group
dependency	= FMS_CPF_SERVER
restart		= YES
affectgroup	= YES
desc		= CPS Backup/Restore Loader
service		= CPS_BUAP_loader
stoptime	= 2000
options		= 

[CPS_BUAP_parmgr]
resourcegroup	= Disk Group
dependency	= FMS_CPF_SERVER
restart		= YES
affectgroup	= YES
desc		= CPS Backup/Restore Parameter Manager
service		= CPS_BUAP_parmgr
stoptime	= 2000
options		= 

