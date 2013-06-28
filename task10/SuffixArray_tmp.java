package com.optprak.suffix;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.text.DecimalFormat;
import java.util.HashMap;
import java.util.Map;

public class SuffixArray {

	public int[] suf;
	public int[] pos;
	public boolean[] bucketStart;
	public boolean[] bucketStart2;
	public int[] bucketSize;
	public String text;
	public int n;
	public Map<String, Integer> bucket;
	Map<Integer, String> posMap;

	private String read(String text_file) throws IOException {
		BufferedReader reader = new BufferedReader(new FileReader(new File(
				text_file)));

		StringBuilder text = new StringBuilder();
		String line;
		while ((line = reader.readLine()) != null) {
			text.append(line);
			if (!line.endsWith("$")) {
				text.append("\r");
			}
			
		}

		reader.close();
		return text.toString();
	}
	
	private String[] readPattern(String pattern_file) throws IOException {
		BufferedReader reader = new BufferedReader(new FileReader(new File(
				pattern_file)));

		String line;
		StringBuilder pattern = new StringBuilder();
		while ((line = reader.readLine()) != null) {
			pattern.append(line);
			if (!line.endsWith("$")) {
				pattern.append("\r");
			}
		}
		String[] p =  pattern.toString().split("\\$");
		for (int i = 0; i < p.length; i++) {
			p[i] = p[i] + "$";
		}

		reader.close();
		return p;
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
		posMap = new HashMap<Integer, String>(n + 1);

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
			//system.out.println(currentChar);
			int bucketContent = bucket.get(currentChar);
			bucket.put(currentChar, i);
			pos[i] = bucketContent;
		}

		//system.out.println("bucket");
		//system.out.println(bucket);

		//system.out.println("pos");
		for (int key : pos) {
			//system.out.print(key + " ");
		}
		//system.out.println();

		int c = 0;
		for (int a = 0; a < alphabet.length; a++) {
			char d = alphabet[a];
			//system.out.println(d);
			int i = bucket.get(String.valueOf(d));
			//system.out.println(i);
			while (i != -1) {
				int j = pos[i];
				//system.out.println(j);
				//system.out.println(c);
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
			//system.out.print(b);
			//system.out.print(" -> ");
		}
		//system.out.println();

		// more stuff
		for (int h = 0; h < Math.floor(Math.log(n) / Math.log(2)); h++) {
			//system.out.println();
			//system.out.println("round " + h);
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
					//system.out.print(l + " -> " + r + " || ");
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
				posMap.put(i, text.substring(pos[i]));
			}

			
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


	public void search(String y) {

		//y=y.substring(0,y.length()-1);
		
		if (y.compareTo(posMap.get(n-1)) >= 0) {
			System.out.println("here we are " + n);
		} else
		if (y.compareTo(posMap.get(0)) < 0) {
			System.out.println("tada " + 0);
		}else {
		
			int smallestIndex = -1;
			int biggestIndex = -1;
			int left = 0;
			int right = n - 1;
			
			while (right - left > 1) {
				int mid = (int) (Math.ceil((right + left) / 2));
				
				int compare = y.compareTo(posMap.get(mid));
				
				if (y.compareTo(posMap.get(mid)) < 0) {
					right = mid;
						
				} else { 
					
					left = mid;
				}
			}
			System.out.println("right is " + right);
			System.out.println("erstes vorkommen" + pos[right]);
		}
		
		
		if (y.compareTo(posMap.get(n-1)) >= 0) {
			System.out.println("here we are2 " + n);
		} else
		if (y.compareTo(posMap.get(0)) < 0) {
			System.out.println("tada2 " + 0);
		}else {
		
			int smallestIndex = -1;
			int biggestIndex = -1;
			int left = 3;
			int right = n-1;
			
			while (right - left > 1) {
				int mid = (int) (Math.ceil((right + left) / 2));
				
				
				if (y.compareTo(posMap.get(mid)) < 0) {
					right = mid;
						
				} else { 
					
					left = mid;
				}
				
				System.out.println(y + " vs " +posMap.get(mid).substring(0, y.length()-1) + "$");
				if (y.equalsIgnoreCase(posMap.get(mid).substring(0, y.length()-1) + "$")) {
					System.out.println("break " + mid);
					left = mid;
					break;
				}
					
			}
			System.out.println("left is2 " + left);
			
			System.out.println("letzes vorkommen " + pos[left]);
		}
		
		/*
		for (int i = 0; i < 10; i++) {
			System.out.println(posMap.get(i));
		}*/
		/**
		System.out.println(pos[153]);
		System.out.println(pos[4]);
		System.out.println(pos[5]);
		System.out.println("###");
		System.out.println(text.substring(pos[153]));
		System.out.println("###");
		*/
		/**
		smallestIndex = left;
		System.out.println(left);
		
		{
			String suff = posMap.get(smallestIndex);
			if (!(y.length() <= suff.length() && y.equals(suff.substring(0, y.length())))) {
				return new int[0];
			}
		}
		
		
		right = n - 1;
		
		while(left != right) {
			int mid = (int) Math.ceil((double)(right + left) / 2.0d);

			String suff = posMap.get(mid);
			if (y.length() <= suff.length() && y.equals(suff.substring(0, y.length()))) {
				left = mid;
			} else {
				right = mid - 1;
			}
			
		}
		
		biggestIndex = right;
		System.out.println(right);
		
		
		int[] result = new int[biggestIndex - smallestIndex + 1];
		for (int i = 0; i < result.length; i++) {
			result[i] = pos[i + smallestIndex];
		}
		return result;
		*/
		
	}
			


	public static void main(String[] args) throws IOException {
		String text_file;
		String pattern_file;
		if(args.length != 2) {
			text_file = "text1.txt";
			pattern_file = "text1.pat";
		} else {
			text_file = args[0];
			pattern_file = args[1];
		}
		
		DecimalFormat df = new DecimalFormat("0.0#########");
		
		
		SuffixArray suff = new SuffixArray();
		String text = suff.read(text_file);
		//text = "bccaababababaab$";
		String[] patterns = suff.readPattern(pattern_file);
		
		
		long time = System.nanoTime();
		suff.suffix_array(text, text.length());
		for (int i : suff.pos) {
			System.out.println(i + " -> " + suff.posMap.get(i));
		}
		System.out.println(suff.pos[157]);
		System.out.println(suff.pos[158]);
		System.out.println(suff.pos[159]);
		System.out.println("Suffix-Array: \tZeit " + df.format(0.000000001d * (System.nanoTime() - time)) + " Sekunden");
		int count = 0;
		for (String pattern : patterns) {
			//pattern = "ab$";
			
			count++;
			
			time = System.nanoTime(); 
			/*int[] result = */suff.search(pattern);
			System.exit(1);
			System.out.println(pattern);
			
			int maxIndex = -1;
			int minIndex = -1;
			/**
			for (int i = 0; i < result.length; i ++) {
				if (maxIndex  == -1 || result[maxIndex] < result[i]) {
					maxIndex = i;
				}
				if (minIndex  == -1 || result[minIndex] > result[i]) {
					minIndex = i;
				}
			}
					
			System.out.println("Muster " + count + ": \t" + result.length + " Vorkommen" + 
					(result.length > 0 ? ("\n\t\tlex. kleinste Pos. " + (result[minIndex]+1) + ", lex. groesste Pos. " + (1+result[maxIndex])) : "") + 
					"\n\t\tZeit: " + df.format(0.000000001d * (System.nanoTime() - time)) + " Sekunden");
			*/
		}
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
