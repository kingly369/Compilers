int printf(char *s, ...);

int main(void)
{
		int a,b,c;
			double x,y,z;
				a = 1;
					b = 2;
						c = 2;

							x = 1.5;
								y = 1.5;
									z = 3.5;
										
										printf("Integer '==' and '!='\n");
											if(a == c && b == c){
														printf("Bad && \n");
															}

												if(a == c && b != c){
															printf("Bad && \n");
																}
													if(a != c && b == c){
																printf("Good &&\n");
																	}
														if(a != c && b != c){
																	printf("Bad && \n");
																		}
															if(a == c || b == c){
																		printf("Good ||\n");
																			}
																if(a == z || b != c){
																			printf("Bad ||\n");
																				}
																	 	
}
