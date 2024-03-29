using System.Diagnostics.CodeAnalysis;

namespace lab72;

[SuppressMessage("ReSharper", "InconsistentNaming")]
public static class StringProvider
{
    public const string MESSAGE_END_ASYNC = ">> Загрузка асинхронно завершена, затрачено времени: ";
    public const string MESSAGE_END_SYNC = ">> Загрузка синхронно завершена, затрачено времени: ";
    public const string MESSAGE_END_DOWNLOAD = "Закончена загрузка ";
    public const string MESSAGE_START_DOWNLOAD = "Началась загрузка изображения ";
    
    public const string ERROR_GET_IMAGE_URL = "Не удалось получить адрес для изображения ";
    public const string ERROR_CAST_ARGUMENT = "Невозможно преобразовать аргумент в число";
    public const string ERROR_NO_ARGUMENT = "Не хватает аргументов. Перезапустите программу с количеством загружаемых картинок";
    public const string ERROR_INVALID_ARGUMENT = "Аргументом должно быть целое положительное число!";
}