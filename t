ssh-keygen -t rsa -C "xuzjun@gmail.com"
cd .ssh
vi id_rsa.pub 
ssh -T git@github.com
git config --global user.name 'xuzjun'
git config --global user.email 'xuzjun@gmail.com'
cd workspace/pro/
git clone https://github.com/xuzjun/pro.git
git add ./src
git add Makefile 
git add test/
git commit
git commit -m 'create'
git push
