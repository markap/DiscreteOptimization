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
		
		// @todo correct size
		int[] suf = new int[n*100];
		int[] pos = new int[n*100];
		boolean[] bucketStart = new boolean[n*100];
		String[] bucketStart2 = new String[n*100];
		String[] bucketSize = new String[n*100];
		Map<String, Integer> bucket = new HashMap<>(n*100);
		
		for (char c = 32; c < 127; c++) {
			bucket.put(String.valueOf(c), -1);
		}
		
		for (int i = 0; i < n; i++) {
			String currentChar = String.valueOf(text.charAt(i));
			int bucketContent = bucket.get(currentChar);
			bucket.put(currentChar, i);
			pos[i] = bucketContent;
		}
		
		System.out.println("bucket");
		for (Map.Entry<String, Integer> entry : bucket.entrySet()) {
			System.out.println(entry.getKey() + " -> " + entry.getValue());
		}
		
		System.out.println("pos");
		for (int key : pos) {
			System.out.print(key + " ");
		}
		System.out.println();
		
		int c = 1;
		for (char d = 32; d < 127; d++) {
			int i = bucket.get(String.valueOf(d));
			while (i != -1) {
				int j = pos[i];
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
		
		
		
		
		// more stuff
		for (int h = 0; h < Math.floor(Math.log(n)/Math.log(2)); h++) {
			
		}
		
	}

	
	
	public static void main(String[] args) throws IOException {
		
		String text = "mein name ist martin$";
		
		SuffixArray suff = new SuffixArray();
		suff.suffix_array(text, text.length());
	}

}
