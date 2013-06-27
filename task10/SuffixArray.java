package com.optprak.suffix;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.NavigableMap;
import java.util.TreeMap;

public class SuffixArray {

	public int[] suf;
	public int[] pos;
	public boolean[] bucketStart;
	public boolean[] bucketStart2;
	public int[] bucketSize;
	public String text;
	public int n;
	public Map<String, Integer> bucket;
	NavigableMap<String, Integer> posMap;

	private void read() throws IOException {
		BufferedReader reader = new BufferedReader(new FileReader(new File(
				"text1.txt")));

		String line;
		while ((line = reader.readLine()) != null) {
			System.out.println(line);
		}

		reader.close();
	}

	private void suffix_array(String text, int n) {

		this.text = text;
		this.n = n;
		suf = new int[n];
		pos = new int[n];
		bucketStart = new boolean[n + 1];
		bucketStart2 = new boolean[n + 1];
		bucketSize = new int[n + 1];

		bucket = new HashMap<>(n + 1);
		posMap = new TreeMap<String, Integer>();

		/*
		 * for (char c = 32; c < 127; c++) { bucket.put(String.valueOf(c), -1);
		 * }
		 */
		char[] alphabet = new char[256];
		for (int i = 0; i < 256; i++) {
			alphabet[i] = (char) i;
		}

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

		int c = 0;
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
		for (int h = 0; h < Math.floor(Math.log(n) / Math.log(2)); h++) {
			System.out.println();
			System.out.println("round " + h);
			int r = 0;
			int l = 0;
			boolean r_found = false;
			boolean l_found = false;
			// search for l and r
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
							if (k > suf[d]
									&& (bucketStart[k] || !bucketStart2[k])) {
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
			for (int i = 0; i < n; i++) {
				posMap.put(text.substring(pos[i]),i);
			}

			
		}

		System.out.println();
		System.out.println("pos");
		for (int i : pos) {
			System.out.println(i);
		}

	}

	public boolean compareGreatest(String y, String x) {

		int ylength = y.length()-1;
		int xlength = x.length()-1;
		for (int i = 0; i < xlength && i < ylength; i++)
			if (y.charAt(i) != x.charAt(i))
				return y.charAt(i) < x.charAt(i);
		return false;

	}
	
	public boolean compareSmallest(String y, String x) {

		int ylength = y.length();
		int xlength = x.length();
		for (int i = 0; i < xlength && i < ylength; i++)
			if (y.charAt(i) != x.charAt(i))
				return y.charAt(i) < x.charAt(i);
		return true;

	}


	public int[] search(String y) {

		
		if (y.compareTo(text.substring(pos[n-1])) > 0) {
			return new int[0];
		}
		if (y.compareTo(text.substring(pos[0])) < 0) {
			return new int[0];
		}
		
		int smallestIndex = -1;
		int biggestIndex = -1;
		int left = 0;
		int right = n - 1;
		
		while (left != right) {
			int mid = (right + left) / 2;
			
			int compare = y.compareTo(text.substring(pos[mid]));
			
			if (compare == 0) {
				left = mid;
				break;	
			} else if (compare > 0) { 
				// y > suff
				left = mid + 1;
			} else { 
				// y < suff
				right = mid;
			}
		}
		
		smallestIndex = left;
		
		{
			String suff = text.substring(pos[smallestIndex]);
			if (!(y.length() <= suff.length() && y.equals(suff.substring(0, y.length())))) {
				return new int[0];
			}
		}
		
		
		right = n - 1;
		
		while(left != right) {
			int mid = (int) Math.ceil((double)(right + left) / 2.0d);

			String suff = text.substring(pos[mid]);
			if (y.length() <= suff.length() && y.equals(suff.substring(0, y.length()))) {
				left = mid;
			} else {
				right = mid - 1;
			}
			
		}
		
		biggestIndex = right;
		
		
		int[] result = new int[biggestIndex - smallestIndex + 1];
		for (int i = 0; i < result.length; i++) {
			result[i] = pos[i + smallestIndex];
		}
		return result;
		
		/*
		int l = 1; 
		int r = n + 1;
		while (l < r) {
			int mid = (l+r)/2;
			if (y.compareTo(text.substring(pos[mid-1])) > 0) {
				l = mid + 1;
				System.out.println(y + " > " + text.substring(pos[mid-1]));
			} else {
				r = mid;
				System.out.println(y + " <= " + text.substring(pos[mid-1]));
			}
		}
		
		System.out.println();
		
		int s = l;
		r = n + 1;
		l = 1;
		while (l < r) {
			int mid = (int) Math.floor((l+r)/2.0d);
			if (y.equals(text.substring(pos[mid-1], Math.min(text.length()-1, pos[mid-1]+y.length()) ))) {
				l = mid;
				System.out.println(y + " == " + text.substring(pos[mid-1], pos[mid-1]+y.length()));
			} else {
				r = mid - 1;
				System.out.println(y + " != " + text.substring(pos[mid-1], pos[mid-1]+y.length()));
			}
		}
		
		System.out.println();
		
		if (s > n || r < 1) {
			return new int[0];
		}
		
		int [] result= new int [r-s+1];
		for (int i=0; i<r-s+1;i++) {
			result[i]= pos[i+s-1];
		}
		return result; 
		*/
	}
			
		
		
		
		
		
		
		
		
	
	
	
	

	public static void main(String[] args) throws IOException {
// Heute programmieren wir einen Algorithmus zur Suche in Texten.
		String text = "bccaababa$";

		SuffixArray suff = new SuffixArray();
		suff.suffix_array(text, text.length());
		
		int[] result = suff.search("caab");
		
		
		int maxIndex = -1;
		int minIndex = -1;
		for (int i = 0; i < result.length; i ++) {
			if (maxIndex  == -1 || result[maxIndex] < result[i]) {
				maxIndex = i;
			}
			if (minIndex  == -1 || result[minIndex] > result[i]) {
				minIndex = i;
			}
		}
				
		System.out.println("\n============\n" + result.length + " Vorkommen" + 
				(result.length > 0 ? ("\nlex. kleinste Pos. " + result[minIndex] + ", lex. groesste Pos. " + result[maxIndex]) : ""));
	}
}
	
/*



		
		if (compareSmallest(y, text.substring(pos[0]))) {
			return new int[] {-1,-1};
		} else if (!(compareSmallest(y, text.substring(pos[n-1])))) {
			System.out.println("x");
			
			return new int[] {n+1,n+1};
		}
		else {
			int [] result= new int[2];
			int l= 0;
			int r= n-1;
			while ((r-l) >1 ) {
				int m= (int) Math.ceil((double)((l+r))/2.0d);
				if(compareGreatest(y,text.substring(pos[m]))) {
					r=m;
				}
				else {
					l=m;
				}
			}
			result[1]=pos[l];
			l= 0;
			r= n-1;
			while ((r-l) >1 ) {
				int m= (int) Math.ceil((double)((l+r))/2.0d);
				if(compareSmallest(y,text.substring(pos[m]))){
					r=m;
				}
				else {
					l=m;
				}
			}
			result[0]=pos[l];
			return result;
		}


	*/
