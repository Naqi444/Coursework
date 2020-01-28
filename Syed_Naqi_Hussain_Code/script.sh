#! /bin/bash
for ((i=1;i<=100;i++))
do
  ./client 127.0.0.1 8787 .2 Transactions.txt;
done
