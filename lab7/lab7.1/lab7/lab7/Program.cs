namespace lab7;

internal static class Program
{
    private static HashSet<char>? GetSkipSymbols()
    {
        Console.Write(StringProvider.MESSAGE_WRITE_REMOVE_SYMBOLS);
        var skipSymbols = Console.ReadLine()?.Trim().ToHashSet();
        return skipSymbols;
    }

    private static List<string> GetFileStrings(string fileDir)
    {
        var streamReader = new StreamReader(fileDir);
        var line = streamReader.ReadLine();
        var lines = new List<string>();

        while (line != null)
        {
            lines.Add(line);
            line = streamReader.ReadLine();
        }

        streamReader.Close();

        return lines;
    }

    private static List<string> GetSkipSymbolsStrings(string fileDir)
    {
        var strings = GetFileStrings(fileDir);
        var skipSymbols = GetSkipSymbols();
        if (skipSymbols != null)
        {
            Parallel.For(0, strings.Count,
                index =>
                {
                    var currentString = strings[index];
                    for (var i = 0; i < currentString.Length; i++)
                    {
                        if (skipSymbols.Contains(currentString[i]))
                        {
                            currentString = currentString.Remove(i, 1);
                        }
                    }

                    strings[index] = currentString;
                });
        }

        return strings;
    }

    private static async Task WriteToFileAsync(List<string> stringsToWrite, string fileDir)
    {
        try
        {
            var streamWriter = new StreamWriter(fileDir);
            foreach (var line in stringsToWrite)
            {
                await streamWriter.WriteLineAsync(line);
            }
            
            streamWriter.Close();
        }
        catch (IOException)
        {
            Console.WriteLine(StringProvider.ERROR_OPEN_FILE_WRITE);
        }
    }

    private static async Task<int> Main(string[] args)
    {
        if (args.Length < 1)
        {
            Console.WriteLine(StringProvider.ERROR_NO_ARGUMENTS);
            return 0;
        }

        try
        {
            var fileDir = args[0];
            var skipSymbolsStrings = GetSkipSymbolsStrings(fileDir);
            var writeToFile = WriteToFileAsync(skipSymbolsStrings, fileDir);

            await writeToFile;
        }
        catch (IOException)
        {
            Console.WriteLine(StringProvider.ERROR_INVALID_FILE);
            return 1;
        }
        catch (IndexOutOfRangeException)
        {
            Console.WriteLine(StringProvider.ERROR_SOMETHING_WENT_WRONG);
            return 1;
        }
        
        Console.WriteLine(StringProvider.MESSAGE_SUCCESS);

        return 0;
    }
}