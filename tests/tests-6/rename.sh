a=1
for i in *.txt; do
  new=$(printf "test-%d.txt" "$a") #04 pad to length of 4
  mv -- "$i" "$new"
  let a=a+1
done
