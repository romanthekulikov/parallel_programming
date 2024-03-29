namespace lab72;

[Obsolete("Obsolete")]
internal static class Program
{
    private static readonly IRepository Repository = new RepositoryImpl();

    private static async Task DoAsyncLoad(int imageNumber)
    {
        var time = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds();

        var imageUrls = await Repository.GetImageUrlsAsync(imageNumber);
        var taskList = imageUrls.Select(url => Repository.LoadImageAsync(url)).ToList();
        await Task.WhenAll(taskList);

        Console.WriteLine(StringProvider.MESSAGE_END_ASYNC + (DateTimeOffset.UtcNow.ToUnixTimeMilliseconds() - time));
    }

    private static void DoSyncLoad(int imageNumber)
    {
        var time = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds();
        var imageUrls = Repository.GetImageUrls(imageNumber);
        foreach (var url in imageUrls)
        {
            Repository.LoadImage(url);
        }
        Console.WriteLine(StringProvider.MESSAGE_END_SYNC + (DateTimeOffset.UtcNow.ToUnixTimeMilliseconds() - time));
    }

    private static async Task<int> Main(string[] args)
    {
        if (args.Length < 1)
        {
            Console.WriteLine(StringProvider.ERROR_NO_ARGUMENT);
            return 1;
        }
        try
        {
            var imageNumber = int.Parse(args[0]);
            if (imageNumber < 0)
            {
                Console.WriteLine(StringProvider.ERROR_INVALID_ARGUMENT);
                return 1;
            }
            
            await DoAsyncLoad(imageNumber);
            DoSyncLoad(imageNumber);
        }
        catch (FormatException)
        {
            Console.WriteLine(StringProvider.ERROR_CAST_ARGUMENT);
            return 1;
        }

        return 0;
    }
}