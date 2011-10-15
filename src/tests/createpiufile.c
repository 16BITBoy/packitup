int test_createpiufile(){
    printf("Writting a piu test file\n");

    int file = file_open("myfile", 1);
    
    DATA *buffer;
    buffer = (DATA *) malloc(sizeof(DATA));
    
    char piu[3] = "PIU";
    
    buffer->data = piu;
    buffer->size = 3;
    
    if(append_to_file(file, buffer) < 0){
        printf("%s\n", piu_errmsg(piu_errno));
        return 1;
    }
    
    int hsize = FILEINFO_SIZE * 2;

    buffer->data = &hsize;
    buffer->size = sizeof(int);

    if(append_to_file(file, buffer) < 0){
        printf("%s\n", piu_errmsg(piu_errno));
        return 1;
    }

    char fname[256] = "miichero.txt";

    buffer->data = fname;
    buffer->size = 256;

    if(append_to_file(file, buffer) < 0 ){
        printf("%s\n", piu_errmsg(piu_errno));
        return 1;
    }

    unsigned long fsize = 8;

    buffer->data = &fsize;
    buffer->size = sizeof(unsigned long);

    if(append_to_file(file, buffer) < 0){
        printf("%s\n", piu_errmsg(piu_errno));
        return 1;
    }
    
    strcpy(fname,"mifichero2.txt");

    buffer->data = &fname;
    buffer->size = 256;

    if(append_to_file(file, buffer) < 0){
        printf("%s\n", piu_errmsg(piu_errno));
        return 1;
    }
    
    fsize = 16;

    buffer->data = &fsize;
    buffer->size = sizeof(unsigned long);

    if(append_to_file(file, buffer) < 0){
        printf("%s\n", piu_errmsg(piu_errno));
        return 1;
    }
    return 0;
}
