[CPS_BUAP_filemgr]
resourcegroup	= RGAPIO
dependency	= CPS_BUAP_parmgr
restart		= YES
affectgroup	= YES
desc		= CPS Backup/Restore File Manager
service		= CPS_BUAP_filemgr
options		= 

[CPS_BUAP_loader]
resourcegroup	= RGAPIO
dependency	= CPS_BUAP_parmgr
restart		= YES
affectgroup	= YES
desc		= CPS Backup/Restore Loader
service		= CPS_BUAP_loader
options		= 

[CPS_BUAP_parmgr]
resourcegroup	= RGAPIO
dependency	= FMS_CPF_SERVER
restart		= YES
affectgroup	= YES
desc		= CPS Backup/Restore Parameter Manager
service		= CPS_BUAP_parmgr
options		= 

