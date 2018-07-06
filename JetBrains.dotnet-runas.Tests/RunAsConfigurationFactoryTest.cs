namespace JetBrains.RunAs.Tests
{
    using System.Collections;
    using System.Collections.Generic;
    using Moq;
    using Shouldly;
    using Xunit;

    public class RunAsConfigurationFactoryTest
    {
        [Theory]
        [ClassData(typeof(TestData))]
        public void ShouldCreateConfiguration(
            string[] args,
            bool success,
            string userName,
            string password,
            IEnumerable<string> runAsArguments,
            IEnumerable<string> commandArguments)
        {
            // Given
            var environment = new Mock<IEnvironment>();
            environment.SetupGet(i => i.Args).Returns(args);
            var configurationFactory = new ConfigurationFactory(environment.Object);
            Configuration configuration = null;
            ToolException exception = null;

            // When
            try
            {
                configuration = configurationFactory.Create();
            }
            catch (ToolException ex)
            {
                exception = ex;
            }

            // Then
            if (success)
            {
                configuration.ShouldNotBeNull();
                configuration.UserName.ShouldBe(userName);
                configuration.Password.ShouldBe(password);
                configuration.RunAsArguments.ShouldBe(runAsArguments);
                configuration.CommandArguments.ShouldBe(commandArguments);
            }
            else
            {
                exception.ShouldNotBeNull();
            }
        }

        private class TestData : IEnumerable<object[]>
        {
            public IEnumerator<object[]> GetEnumerator()
            {
                yield return new object[] { new [] {"-u:username", "-p:password"}, true, "username", "password", new string[] {}, new string[] {} };
                yield return new object[] { new [] {"-il:high", "-u:username", "-p:password", "build"}, true, "username", "password", new[] {"-il:high"}, new[] {"build"} };
                yield return new object[] { new [] {"-il:high", "-u:username", "-p:password", "-c:args.txt", "build"}, true, "username", "password", new[] {"-il:high", "-c:args.txt"}, new[] {"build"} };
                yield return new object[] { new [] {"-il:high", "-p:password", "build"}, false, null, null, null, null };
                yield return new object[] { new [] {"-il:high", "-u:username", "build"}, false, null, null, null, null };
                yield return new object[] { new string[] {}, false, null, null, null, null };
            }

            IEnumerator IEnumerable.GetEnumerator() => GetEnumerator();
        }
    }
}
