package com.optprak.suffix;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

public class SuffixArray {
	
	private void read() throws IOException {
		BufferedReader reader = new BufferedReader(new FileReader(new File("text1.txt")));
		
		String line;
		while ((line = reader.readLine()) != null) {
			System.out.println(line);
		}
		
		
		
		reader.close();
	}
	
	private void suffix_array(String text, int n) {
		
		int[] suf = new int[n+1];
		int[] pos = new int[n+1];
		boolean[] bucketStart = new boolean[n+1];
		boolean[] bucketStart2 = new boolean[n+1];
		int[] bucketSize = new int[n+1];
		Map<String, Integer> bucket = new HashMap<>(n+1);
		
		/*for (char c = 32; c < 127; c++) {
			bucket.put(String.valueOf(c), -1);
		}*/
		char[] alphabet = {'a', 'b', 'c', '$'};
		for (int i = 0; i < alphabet.length; i++) {
			bucket.put(String.valueOf(alphabet[i]), -1);
		}
		
		for (int i = 0; i < n; i++) {
			String currentChar = String.valueOf(text.charAt(i));
			System.out.println(currentChar);
			int bucketContent = bucket.get(currentChar);
			bucket.put(currentChar, i); 
			pos[i] = bucketContent;
		}
		
		System.out.println("bucket");
		System.out.println(bucket);
		
		System.out.println("pos");
		for (int key : pos) {
			System.out.print(key + " ");
		}
		System.out.println();
		
		
		int c = 1;
		for (int a = 0; a < alphabet.length; a++) {
			char d = alphabet[a];
			System.out.println(d);
			int i = bucket.get(String.valueOf(d));
			System.out.println(i);
			while (i != -1) {
				int j = pos[i];
				System.out.println(j);
				System.out.println(c);
				suf[i] = c;
				if (i == bucket.get(String.valueOf(d))) {
					
					bucketStart[c] = true;
				} else {
					
					bucketStart[c] = false;
				}
				c++;
				i = j;
			}
		}
		
		bucketStart[n] = true;
		for (int i = 0; i < n; i++) {
			pos[suf[i]] = i;
		}
		
		
		for (boolean b : bucketStart) {
			System.out.print(b);
			System.out.print(" -> ");
		}
		System.out.println();
		
		
		
		
		// more stuff
		for (int h = 0; h < Math.floor(Math.log(n)/Math.log(2)); h++) {
			System.out.println();
			System.out.println("round " +h);
			int r = 0;
			int l = 0;
			boolean r_found = false;
			boolean l_found = false;
			for (int g = 0; g < bucketStart.length; g++) {
				
				if (r_found == true && l_found == true) {
					r_found = false;
					l = r;
					
				}
				boolean b = bucketStart[g];
				if (b == true && l_found == false) {
				
					l = g;
					l_found = true;
					continue;
				}
				if (b == true && l_found == true) {
					r_found = true;
					r = g;
					System.out.print(l + " -> " + r + " || ");
					bucketSize[l] = 0;
					for (int i = l; i < r; i++) {
						suf[pos[i]] = l;
					}
				}
			}
			
			
			r = 0;
			l = 0;
			r_found = false;
			l_found = false;
			for (int g = 0; g < bucketStart.length; g++) {
				
				if (r_found == true && l_found == true) {
					r_found = false;
					l = r;
				}
				boolean b = bucketStart[g];
				if (b == true && l_found == false) {
				
					l = g;
					l_found = true;
					continue;
				}
				if (b == true && l_found == true) {
					r_found = true;
					r = g;
					System.out.print(l + " -> " + r + " || ");
					for (int i = l; i < r; i++) {
						int d = (int) (pos[i] - Math.pow(2, h));
						if (d < 0 || d >= n) {
							continue;
						}
						int k = suf[d];
						suf[d] = k + bucketSize[k];
						bucketSize[k]++;
						bucketStart2[suf[d]] = true;
					}
					for (int i = l; i < r; i++) {
						int d = (int) (pos[i] - Math.pow(2, h));
						if (d < 0 || d >= n || !bucketStart[suf[d]]) {
							continue;
						}
						int k = 0;
						while (true) {
							if (k > suf[d] && (bucketStart[k] || !bucketStart2[k])) {
								break;
							}
							k++;
						}
						for (int j = suf[d] + 1; j < k; j++) {
							bucketStart2[j] = false;
						}
						
						
					}

				}
			}
			for (int i = 0; i < n; i++) {
				pos[suf[i]] = i;
				bucketStart[i] = bucketStart[i] || bucketStart2[i];
			}
		}
		
		System.out.println();
		System.out.println("pos");
		for (int i : pos) {
			System.out.println(i);
		}
		
	
	}

	
	
	public static void main(String[] args) throws IOException {
		
		String text = "bccaababa$";
		
		SuffixArray suff = new SuffixArray();
		suff.suffix_array(text, text.length());
	}

}
