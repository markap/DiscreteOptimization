package com.optprak.suffix;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.text.DecimalFormat;
import java.util.Arrays;
import java.util.Comparator;


// Algorithm to find string in text
// Building suffix array in O(n log n) time (n is length of text)
// Search algorithm runs in O(m log n) time (m is length of string y it is being searched for)
public class SuffixArray {
	public static DecimalFormat df = new DecimalFormat("0.0#########");
	static String text;
	
	Integer[] suf, pos; //output, suffix and position array
	int[] bucketSize, next; //internal
	boolean[] bucketStart, bucketStart2;
	 
	public static Comparator<Integer> FruitNameComparator 
    = new Comparator<Integer>() {

		@Override
		public int compare(Integer a, Integer b) {//compare characters at position a and b
			// TODO Auto-generated method stub
			return text.charAt(a) - text.charAt(b);
	
		}
	
	};
	 
	private void suffixSort(int n){//initial sorting of the suffixes, n is the length of the text
		suf = new Integer[n];
		pos = new Integer[n];
		bucketSize = new int[n];
		next = new int[n];
		bucketStart = new boolean[n+1];
		bucketStart2 = new boolean[n+1];
		
		
		for (int i=0; i<n; ++i){
		    pos[i] = i;
		}
		 
		//sort pos according to first characters of suffixes
		Arrays.sort(pos, new Comparator<Integer>() {
	
			@Override
			public int compare(Integer a, Integer b) {//compare characters at position a and b
				
				return text.charAt(a) - text.charAt(b);
			
			}
			
		});
	 
	 
	  for (int i=0; i<n; ++i){
	    bucketStart[i] = i == 0 || text.charAt(pos[i]) != text.charAt(pos[i-1]);
	    bucketStart2[i] = false;
	  }
	 //Recursive bucket sort phase
	  for (int h = 1; h < n; h <<= 1){
	    
	    int buckets = 0;
	    for (int i=0, j; i < n; i = j){
	      j = i + 1;
	      while (j < n && !bucketStart[j]) j++;
	      next[i] = j;
	      buckets++;
	    }
	    if (buckets == n) break; // We are done! 
	    //suffixes are separated in buckets containing strings starting with the same h characters}
	 
	    for (int i = 0; i < n; i = next[i]){
	      bucketSize[i] = 0;
	      for (int j = i; j < next[i]; ++j){
	        suf[pos[j]] = i;
	      }
	    }
	 
	    bucketSize[suf[n - h]]++;
	    bucketStart2[suf[n - h]] = true;
	    for (int i = 0; i < n; i = next[i]){
	      for (int j = i; j < next[i]; ++j){
	        int s = pos[j] - h;
	        if (s >= 0){
	          int head = suf[s];
	          suf[s] = head + bucketSize[head]++;
	          bucketStart2[suf[s]] = true;
	        }
	      }
	      for (int j = i; j < next[i]; ++j){
	        int s = pos[j] - h;
	        if (s >= 0 && bucketStart2[suf[s]]){
	          for (int k = suf[s]+1; !bucketStart[k] && bucketStart2[k]; k++) bucketStart2[k] = false;
	        }
	      }
	    }
	    for (int i=0; i<n; ++i){
	      pos[suf[i]] = i;
	      bucketStart[i] |= bucketStart2[i];
	    }
	  }
	  for (int i=0; i<n; ++i){
	    suf[pos[i]] = i;
	  }
	  /*
		for (int i = 0; i < n; i++) {
			//posMap.put(i, text.substring(pos[i]));
			System.out.println(text.substring(pos[i]));
		}
		*/
	 // System.out.println("round");
	}
	
	
	private String read(String text_file) throws IOException { // reading from text file
		BufferedReader reader = new BufferedReader(new FileReader(new File(
				text_file)));

		StringBuilder text = new StringBuilder();
		String line;
		while ((line = reader.readLine()) != null) {
			text.append(line);
			text.append("\r");
		}

		reader.close();
		return text.toString();
	}
	
	private boolean checkCorrectness(int n) { //checking correctness of position array after initial sorting
		boolean result =false;
		for (int i=0; i<n;i++) {
			for (int j=i+1; j<n;j++) {
				String y = text.substring(pos[i]);
				String x = text.substring(pos[j]);
				int ylength = y.length();
				int xlength = x.length();
				for (int k = 0; k < xlength && k < ylength && !result; k++) {
					if (y.charAt(k) != x.charAt(k))
						if (y.charAt(k) > x.charAt(k)) {
							System.out.println("Indexes: "+ i+", "+j+" : "+y +" > "+x);
							return false;
						} else result= true;
				}
				result =false;
				
				

			}
			
		}
		return true;
	}
		
	
	
	private void search(String y, int count, long time) {//binary search to find number of appearances, lexicographically smallest appearance and greatest appearance

		y=y.substring(0,y.length()-1); // delete $ in the end of string
		//System.out.println(y);
		
		boolean appearance=true;// by default appearance is true
		if (y.compareTo(text.substring(pos[text.length() - 1])) > 0) {
			appearance=false;// text does not contain y
		}
		if (y.compareTo(text.substring(pos[0])) < 0) {
			appearance=false;// text does not contain y
		}
		
		int smallestIndex = -1;
		int biggestIndex = -1;
		int left = 0;
		int right = text.length() - 1;
		
		while (left != right) {//search for appearance with smallest index
			int mid = (right + left) / 2;
			
			int compare = y.compareTo(text.substring(pos[mid]));
			//System.out.println("Mid: "+ text.substring(pos[mid]));
			if (compare == 0) {
				left = mid;
				//System.out.println("Break;");
				break;	
			} else if (compare > 0) { 
				// y > suff
				left = mid + 1;
				//System.out.println("Left");
			} else { 
				// y < suff
				right = mid;
				//System.out.println("Right");
			}
		}

		smallestIndex = left;
		//stem.out.println("Smallest Index: " + pos[smallestIndex]);//+text.substring(pos[smallestIndex]));
		
		{
			String suff = text.substring(pos[smallestIndex]);
			if (!(y.length() <= suff.length() && y.equals(suff.substring(0, y.length())))) {
				appearance =false; // text does not contain y
			}
		}
		
		
		right = text.length() - 1;
		
		while(left != right) {//find the appearance with greatest index
			int mid = (int) Math.ceil((double)(right + left) / 2.0d);

			String suff = text.substring(pos[mid]);
			if (y.length() <= suff.length() && y.equals(suff.substring(0, y.length()))) {
				left = mid;
			} else {
				right = mid - 1;
			}
			
		}
		
		biggestIndex = right;
		//System.out.println("Biggest Index: "+ pos[biggestIndex]);//text.substring(pos[biggestIndex]));
		//System.out.println(appearance);
		
		if (appearance == false) {
			//System.out.println("kommt nicht vor");
		} else if (text.substring(pos[smallestIndex]).startsWith(y.substring(0, y.length()-1)) && text.substring(pos[biggestIndex]).startsWith(y.substring(0, y.length()-1))) {
	
			//System.out.println("passt");
		} else {
			System.out.println("passt nicht");
			System.exit(1);
		}
		if (appearance) {
			//System.out.println(y);
			//System.out.println(text.substring(pos[smallestIndex]));
			//System.out.println(text.substring(pos[biggestIndex]));
		}
		
		//Output after binary search
		System.out.println("Muster " + count + ": " +  
				(appearance ?((biggestIndex - smallestIndex + 1) + " Vorkommen")+ (", lex. kleinste Pos. " + (pos[smallestIndex] +1) + ", lex. groesste Pos. " + (pos[biggestIndex]+1)) : ("0" + " Vorkommen")) + 
				", Zeit: " + df.format(0.000000001d * (System.nanoTime() - time)) + " Sekunden");
		
		
		
	}
	
	private String[] readPattern(String pattern_file) throws IOException { // read pattern file
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

	/**
	 * @param args
	 * @throws IOException 
	 */
	public static void main(String[] args) throws IOException {
		
		String text_file;
		String pattern_file;
		if(args.length != 2) {// if no file to open is entered in the command line
			text_file = "text6.txt";
			pattern_file = "text6.pat";
		} else {
			text_file = args[0];
			pattern_file = args[1];
		}
		
		
		SuffixArray arr = new SuffixArray();
		text = arr.read(text_file);
		//text = "hdbhaabdbabab\rb" +"abdh$";
		
		long time = System.nanoTime();// time with nano seconds accuracy
		arr.suffixSort(text.length());
		System.out.println("Suffix-Array: \tZeit " + df.format(0.000000001d * (System.nanoTime() - time)) + " Sekunden");
		
		String[] pattern = arr.readPattern(pattern_file);
	
		int i = 0;
		for (String p : pattern) {
			i++;
			//System.out.println("pattern " + p);
			/**
			for (int i = 0; i < text.length(); i++) {
				if (text.substring(arr.pos[i]).startsWith(p.substring(0, p.length()-1))) {
					System.out.println(i + "->" + arr.pos[i] + "->" + text.substring(arr.pos[i]));
				}
				
			}
			System.exit(1);
			*/
			time = System.nanoTime(); 
			arr.search(p, i, time);
			
			//System.exit(1);
		}

		//System.out.println("done");
		//System.out.println(arr.checkCorrectness(text.length()));
	}

}
