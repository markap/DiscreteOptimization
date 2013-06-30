package com.optprak.suffix;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.text.DecimalFormat;
import java.util.Arrays;
import java.util.Comparator;

public class SuffixArray {
	public static DecimalFormat df = new DecimalFormat("0.0#########");
	static String text;
	
	Integer[] suf, pos; //output
	int[] bucketSize, next; //internal
	boolean[] bucketStart, bucketStart2;
	 
	public static Comparator<Integer> FruitNameComparator 
    = new Comparator<Integer>() {

		@Override
		public int compare(Integer a, Integer b) {
			// TODO Auto-generated method stub
			return text.charAt(a) - text.charAt(b);
	
		}
	
	};
	 
	void suffixSort(int n){
		suf = new Integer[n];
		pos = new Integer[n];
		bucketSize = new int[n];
		next = new int[n];
		bucketStart = new boolean[n+1];
		bucketStart2 = new boolean[n+1];
	  //sort suffixes according to their first characters
	  for (int i=0; i<n; ++i){
	    pos[i] = i;
	  }
	 
	  Arrays.sort(pos, new Comparator<Integer>() {

			@Override
			public int compare(Integer a, Integer b) {
				// TODO Auto-generated method stub
				return text.charAt(a) - text.charAt(b);
		
			}
		
		});
	 
	 
	  for (int i=0; i<n; ++i){
	    bucketStart[i] = i == 0 || text.charAt(pos[i]) != text.charAt(pos[i-1]);
	    bucketStart2[i] = false;
	  }
	 
	  for (int h = 1; h < n; h <<= 1){
	    
	    int buckets = 0;
	    for (int i=0, j; i < n; i = j){
	      j = i + 1;
	      while (j < n && !bucketStart[j]) j++;
	      next[i] = j;
	      buckets++;
	    }
	    if (buckets == n) break; // We are done! 
	    //{suffixes are separted in buckets containing strings starting with the same h characters}
	 
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
	
	
	private String read(String text_file) throws IOException {
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
	
	public boolean checkCorrectness(int n) {
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
		
	
	
	public void search(String y, int count, long time) {

		y=y.substring(0,y.length()-1);
		//System.out.println(y);
		
		boolean appearance=true;
		if (y.compareTo(text.substring(pos[text.length() - 1])) > 0) {
			appearance=false;
		}
		if (y.compareTo(text.substring(pos[0])) < 0) {
			appearance=false;
		}
		
		int smallestIndex = -1;
		int biggestIndex = -1;
		int left = 0;
		int right = text.length() - 1;
		
		while (left != right) {
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
				appearance =false;
			}
		}
		
		
		right = text.length() - 1;
		
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
		//System.out.println("Biggest Index: "+ pos[biggestIndex]);//text.substring(pos[biggestIndex]));
		//System.out.println(appearance);
		
		if (appearance == false) {
			System.out.println("kommt nicht vor");
		} else if (text.substring(pos[smallestIndex]).startsWith(y.substring(0, y.length()-1)) && text.substring(pos[biggestIndex]).startsWith(y.substring(0, y.length()-1))) {
	
			System.out.println("passt");
		} else {
			System.out.println("passt nicht");
			System.exit(1);
		}
		
		System.out.println("Muster " + count + ": " +  
				(appearance ?((biggestIndex - smallestIndex + 1) + " Vorkommen")+ (", lex. kleinste Pos. " + (pos[smallestIndex]) + ", lex. groesste Pos. " + (pos[biggestIndex])) : ("0" + " Vorkommen")) + 
				", Zeit: " + df.format(0.000000001d * (System.nanoTime() - time)) + " Sekunden");
		
		
		
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

	/**
	 * @param args
	 * @throws IOException 
	 */
	public static void main(String[] args) throws IOException {
		
		// TODO Auto-generated method stub
		SuffixArray arr = new SuffixArray();
		text = arr.read("text1.txt");
		//text = "hdbhaabdbabab\rb" +"abdh$";
		
		long time = System.nanoTime();
		arr.suffixSort(text.length());
		System.out.println("Suffix-Array: \tZeit " + df.format(0.000000001d * (System.nanoTime() - time)) + " Sekunden");
		
		String[] pattern = arr.readPattern("text1.pat");
	
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
