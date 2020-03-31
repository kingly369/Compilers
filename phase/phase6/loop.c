/* hello.c */

int printf(char *s, ...);

int main(void)
{
	    int i,j,k;
		    for( i = 0; i < 10; i++){
				        j = i+1;
						        while(i < j){
									            printf("Printing j: %d", j);
												            for(k = 6; k > 0; k--){
																                if(k == i)
																					                    break;
																				                printf("Printing k: %d", k);
																								            }
															            j--;
																		            if(j == 5)
																						                break;
																					        }
								    }
}
