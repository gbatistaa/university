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

      // Início do tempo
      long startTime = System.nanoTime();

      // Medição de memória antes
      Runtime runtime = Runtime.getRuntime();
      runtime.gc(); // Garante coleta de lixo antes da medição
      long usedMemoryBefore = (runtime.totalMemory() - runtime.freeMemory()) / (1024 * 1024); // em MB

      Thread showArrayThread = new ShowArrayThread(array);
      Thread sortThread = new SortThread(array);

      showArrayThread.start();
      sortThread.start();

      try {
        showArrayThread.join();
        sortThread.join();
      } catch (InterruptedException e) {
        e.printStackTrace();
      }

      // Fim do tempo
      long endTime = System.nanoTime();
      long elapsedTimeMs = (endTime - startTime) / 1_000_000;

      // Medição de memória depois
      long usedMemoryAfter = (runtime.totalMemory() - runtime.freeMemory()) / (1024 * 1024); // em MB

      System.out.println("\n--- Estatísticas ---");
      System.out.println("Tempo de execução: " + elapsedTimeMs + " ms");
      System.out.println("Memória usada antes: " + usedMemoryBefore + " MB");
      System.out.println("Memória usada depois: " + usedMemoryAfter + " MB\n");
    }

    scanner.close();
  }
}
