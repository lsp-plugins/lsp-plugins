function binomialTest{T <: Integer}(n::T, k::T)

    output = 1

    if (k < 0)
        return 0
    end

    if n < 0

        n = -n + k - 1

        if isodd(k)
            output = -1
        end

    end

    if n < k
        return 0
    end

    if (k == 0) || (k == n)
        return output
    end

    if k <= (n >> 2)
        iterations = k
    else
        iterations = n - k
    end

    for it = 1:iterations

        output *= ((n + 1 - it) / it)

    end

    return round(T, output)

end
