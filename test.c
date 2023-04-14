int main(){
	printf(1, "test executing\n");
	if(setuid(0) < 0){
		printf(1, "setuid failed");
	}
	if (seteuid(0) <0){
		printf(1, "seteuid failed");	
	}
	exit();
}
