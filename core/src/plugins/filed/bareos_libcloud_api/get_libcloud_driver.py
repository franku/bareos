import ctypes
import libcloud
from multiprocessing.sharedctypes import RawArray

options = {
    "secret": "minioadmin",
    "key": "minioadmin",
    "host": "127.0.0.1",
    "port": 9000,
    "secure": False,
    "buckets_include": "core",
}


def get_driver(options):
    driver_opt = dict(options)

    # remove unknown options
    for opt in (
        "buckets_exclude",
        "accurate",
        "nb_worker",
        "prefetch_size",
        "queue_size",
        "provider",
        "buckets_include",
        "debug",
    ):
        if opt in options:
            del driver_opt[opt]

    driver = None

    provider = getattr(libcloud.storage.types.Provider, "S3")
    driver = libcloud.storage.providers.get_driver(provider)(**driver_opt)

    try:
        driver.get_container("123invalidname123")

    # success
    except libcloud.storage.types.ContainerDoesNotExistError:
        return driver

    # error
    except libcloud.common.types.InvalidCredsError:
        print("Wrong credentials")
        pass

    # error
    except:
        print("Error connecting driver")
        pass

    print(
        "Could not connect to libcloud driver: %s:%d"
        % (driver_opt["host"], driver_opt["port"])
    )

    return None


if __name__ == "__main__":
    driver = get_driver(options)

    if driver == None:
        exit(1)

    print("Success")