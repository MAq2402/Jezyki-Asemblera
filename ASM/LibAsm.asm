.code

MyProc1 proc x: QWORD, y: QWORD

	xor rax,rax
	mov rax, x
	add rax, y
	ret

MyProc1 endp

MyProc2 proc
	mov eax, 99
	ret
MyProc2 endp

MyProc3 proc ; rcx - piewrsza bitmapa, rdx - druga bitmapa r8 - rozmiar, rax-wynik
	mov r10, rcx ; wskaznik na pierwsza bitmape do r10  
	add r10, r8;  wskaznik na koniec pierwszej bitmapy
	;pomysl o zmiennych pomocnicznych do przesuwania petli
loop1:
	cmp rcx, r10; porownanie  
	jnl end_loop1 ; jesli koniec tablicy wyskocz z petli
	movups xmm1,[rcx]; pierwsza bitmapa do  xmm1
	movups xmm2,[rdx]; druga bitmapa do xmm2
	mov r11, 00000000000000002h ; zapis wartosci 2 do r11
	vmovq xmm5, r11; 2 do xmm5

	;dzielenie i dodawaniie
	vdivps xmm1, xmm1,xmm5;
	vdivps xmm2, xmm2,xmm5
	vpaddd xmm1,xmm2,xmm1;
	;vdivps xmm1, 2,xmm1
	;vdivps xmm2, 2,xmm2
	;vpaddd xmm1,xmm2,xmm1;


	movups [rax], xmm1; zapis do resultBitmap

	; przesuniecie wskaznika
	add rcx,08h;
	add rdx,08h;
	add rax,08h;
	jmp loop1
end_loop1:
	ret
MyProc3 endp

end