import java.util.ArrayList;
import java.util.Scanner;

public class App {
  public static void main(String[] args) {
    System.out.println("Hello, World!");
    Scanner scanner = new Scanner(System.in);

    while (true) {
      System.out.print("Vector size: ");
      int arraySize = scanner.nextInt();
      if (arraySize == 0)
        break;

      System.out.println();

      ArrayList<Integer> array = new ArrayList<Integer>();
      array.ensureCapacity(arraySize);

      for (int i = 0; i < arraySize; i++) {
        System.out.print("Element " + (i + 1) + ": ");
        int newElement = scanner.nextInt();
        array.add(newElement);
      }

      Thread showArrayThread = new ShowArrayThread(array);
      Thread sortThread = new SortThread(array);

      showArrayThread.start();
      sortThread.start();

      // Wait for threads to finish before next input
      try {
        showArrayThread.join();
        sortThread.join();
      } catch (InterruptedException e) {
        e.printStackTrace();
      }
    }

    scanner.close();
  }
}
