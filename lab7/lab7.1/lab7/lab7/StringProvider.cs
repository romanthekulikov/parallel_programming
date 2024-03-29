using System.Diagnostics.CodeAnalysis;

namespace lab7;
[SuppressMessage("ReSharper", "InconsistentNaming")]
public static class StringProvider
{
    
    public const string MESSAGE_WRITE_REMOVE_SYMBOLS = "Напишите символы для удаления:";
    public const string MESSAGE_SUCCESS = "Файл был успешно обработан!";

    public const string ERROR_OPEN_FILE_WRITE = "Произошла ошибка при открытии файла на запись.";

    public const string ERROR_NO_ARGUMENTS =
        "Не достаточно аргументов. Перезапустите программу с указанием имени входного файла";

    public const string ERROR_INVALID_FILE =
        "Указан невалидный файл. Убедитесь, что файл лежит в папке с ифполняемым файлом или пропишите полный путь к файлу";

    public const string ERROR_SOMETHING_WENT_WRONG = "Что-то пошло не так при выполнении программы :(";
}