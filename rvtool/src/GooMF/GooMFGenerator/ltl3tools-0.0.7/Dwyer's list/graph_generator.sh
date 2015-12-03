#!/bin/bash

./ltl2mon "[]((a && (<>b)) -> (! b U c))" | dot -Tps > n1.ps
./ltl2mon "[]((a && (<>b)) -> (! b U c))" | dot -Tps > n2.ps
./ltl2mon "<>a -> ((! a) U b)" | dot -Tps > n3.ps	
./ltl2mon "[](! a) || <>(a && <>b)" | dot -Tps > n4.ps	
./ltl2mon "(<>a) -> ((! b) U a)" | dot -Tps > n5.ps	
./ltl2mon "[]((a && (<>b)) -> ((! c) U b))" | dot -Tps > n6.ps	
./ltl2mon "[](a -> ((! b) U (c || [](! b))))" | dot -Tps > n7.ps	
./ltl2mon "[]((a && <>b) ->((! c) U b))" | dot -Tps > n8.ps	
./ltl2mon "((! a) U (b || [] (! a)))" | dot -Tps > n9.ps	
./ltl2mon "[](! (a || b || c || d || e))" | dot -Tps > n10.ps	
./ltl2mon "[]((state0 && ! state1 && ! state2 && ! state3) || (! state0 && state1 && ! state2 && ! state3) ||(! state0 && ! state1 && state2 && ! state3) || (! state0 && ! state1 && ! state2 && state3) )" | dot -Tps > n11.ps	
./ltl2mon "[]((a && (<>b)) -> (! (c || d)) U b)" | dot -Tps > n12.ps	
./ltl2mon "[]((a && <>(b || c)) -> (! d) U (b || c))" | dot -Tps > n13.ps	
./ltl2mon "<>a -> ((! b)  U a)" | dot -Tps > n14.ps	
./ltl2mon "[]((a && <>b) -> (! a) U b)" | dot -Tps > n15.ps	
./ltl2mon "[]((a && <>b) -> ! (a || c) U b)" | dot -Tps > n16.ps	
./ltl2mon "[](a -> ! b)" | dot -Tps > n17.ps	
./ltl2mon "[]((a && <> b) -> (c -> (((! d) && (! b)) U f)) U b)" | dot -Tps > n18.ps	
./ltl2mon "(! (a || b)) U (c || [](! (a || b)))" | dot -Tps > n19.ps	
./ltl2mon "[](a -> ((! (c || d)) U (b || [](! (c || d)))))" | dot -Tps > n20.ps	
./ltl2mon "[](a -> ((! (c || d)) U (b || [](! (c || d)))))" | dot -Tps > n21.ps	
./ltl2mon "<>a -> ((! b) U a)" | dot -Tps > n22.ps	
./ltl2mon "[](a -> ((! c) U (b || [](! c))))" | dot -Tps > n23.ps	
./ltl2mon "[]((a && (c || d)) -> ((! ( e || f)) U (b || ([](! ( e || f))))))" | dot -Tps > n24.ps
./ltl2mon "(<> a) -> ((! b) U a)" | dot -Tps > n25.ps	
./ltl2mon "[](a -> ((! b) U (c || [](! b))))" | dot -Tps > n26.ps	
./ltl2mon "[](a -> (b && c))" | dot -Tps > n27.ps
./ltl2mon "(a -> ((! b) U c)) U (b || [](! b))" | dot -Tps > n28.ps
./ltl2mon "[](! (a && b))" | dot -Tps > n29.ps
./ltl2mon "[](p -> (q U r))" | dot -Tps > n30.ps	
./ltl2mon "[](! (a && b))" | dot -Tps > n31.ps
./ltl2mon "[] (a -> ([] (! b) && <>c))" | dot -Tps > n32.ps	
./ltl2mon "<> a" | dot -Tps > n33.ps
./ltl2mon "(p && X q) || (q && X p)" | dot -Tps > n34.ps	
./ltl2mon "<> ((a && b) && b U c)" | dot -Tps > n35.ps	
./ltl2mon "<> (p U q)" | dot -Tps > n36.ps
./ltl2mon "[](f -> []f)" | dot -Tps > n37.ps	
./ltl2mon "<> a" | dot -Tps > n38.ps
./ltl2mon "[](a -> (! <>b))" | dot -Tps > n39.ps	
./ltl2mon "a -> (<>(b || c))" | dot -Tps > n40.ps	
./ltl2mon "[](a -> (! b && ! c))" | dot -Tps > n41.ps
./ltl2mon "[](a -> b)" | dot -Tps > n42.ps
./ltl2mon "[](a -> (! a) U (b || [](! a)))" | dot -Tps > n43.ps
./ltl2mon "[](! a)" | dot -Tps > n44.ps
./ltl2mon "[](a -> b)" | dot -Tps > n45.ps
./ltl2mon "[](a -> b)" | dot -Tps > n46.ps
./ltl2mon "[](a -> b)" | dot -Tps > n47.ps
./ltl2mon "[](a -> !(b U c))" | dot -Tps > n48.ps	
./ltl2mon "[](a && ! b)" | dot -Tps > n49.ps
./ltl2mon "[]((a && (<> b)) -> ((! a) U b))" | dot -Tps > n50.ps	
./ltl2mon "[](a -> (! b))" | dot -Tps > n51.ps
./ltl2mon "[](a -> (b U c))" | dot -Tps > n52.ps	
./ltl2mon "[]((a U (! b)) || []a)" | dot -Tps > n53.ps	
./ltl2mon "[]a" | dot -Tps > n54.ps

