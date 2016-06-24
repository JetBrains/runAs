net user RunAsTestUser /delete
net user /add /active:yes RunAsTestUser aaa
net localgroup Administrators RunAsTestUser /ADD