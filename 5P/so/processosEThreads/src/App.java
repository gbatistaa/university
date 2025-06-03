import java.util.ArrayList;
import java.util.Scanner;

public class App {
  public static void main(String[] args) throws Exception {
    System.out.println("Hello, World!");

    Scanner scanner = new Scanner(System.in);
    System.out.println("Vector size: ");
    int arraySize = 0;

    while (true) {
      arraySize = scanner.nextInt();

      if (arraySize == 0) {
        break;
      }

      ArrayList<Integer> array = new ArrayList<Integer>();
      array.ensureCapacity(arraySize);

      for (int i = 0; i < arraySize; i++) {
        System.out.print("Element " + (i + 1) + ": ");
        int newElement = scanner.nextInt();
        array.add(newElement);
      }
    }

    scanner.close();
  }
}
